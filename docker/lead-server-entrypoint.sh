#!/bin/bash
set -Eeuo pipefail

readonly runtime_root=/srv/lead
readonly template_root=/opt/lead/serverfiles
readonly binary_root=/opt/lead/bin

readonly mariadb_host="${MARIADB_HOST:-db-lead}"
readonly sql_user="${SQL_USER:-root}"
readonly sql_password="${SQL_PASSWORD:-password}"
readonly public_ip="${LEAD_PUBLIC_IP:-127.0.0.1}"
readonly p2p_host="${LEAD_P2P_HOST:-127.0.0.1}"

declare -a child_pids=()

render_sql_credentials()
{
    local config_file="$1"

    sed -i \
        -e "s/127\\.0\\.0\\.1/${mariadb_host}/g" \
        -e "s/ metin2 password / ${sql_user} ${sql_password} /g" \
        "${config_file}"
}

prepare_db()
{
    local work_dir="${runtime_root}/db"

    install -d "${work_dir}/log" "${work_dir}/cores"
    cp "${template_root}/db/conf.txt" "${work_dir}/conf.txt"
    cp "${template_root}/share/conf/item_proto.txt" "${work_dir}/item_proto.txt"
    cp "${template_root}/share/conf/mob_proto.txt" "${work_dir}/mob_proto.txt"
    cp "${template_root}/share/conf/item_names.txt" "${work_dir}/item_names.txt"
    cp "${template_root}/share/conf/mob_names.txt" "${work_dir}/mob_names.txt"

    render_sql_credentials "${work_dir}/conf.txt"
    printf '\nP2P_HOST = "%s"\n' "${p2p_host}" >> "${work_dir}/conf.txt"
}

prepare_game()
{
    local name="$1"
    local template_dir="$2"
    local work_dir="${runtime_root}/${name}"

    install -d "${work_dir}/log" "${work_dir}/cores" "${work_dir}/mark"
    cp "${template_root}/${template_dir}/CONFIG" "${work_dir}/CONFIG"
    ln -sfn "${template_root}/share/data" "${work_dir}/data"
    ln -sfn "${template_root}/share/locale" "${work_dir}/locale"
    ln -sfn "${template_root}/share/conf/CMD" "${work_dir}/CMD"
    : > "${work_dir}/package"

    render_sql_credentials "${work_dir}/CONFIG"
    sed -i "s/^DB_ADDR:.*/DB_ADDR: 127.0.0.1/" "${work_dir}/CONFIG"
    printf '\nBIND_IP: %s\n' "${public_ip}" >> "${work_dir}/CONFIG"
}

start_child()
{
    local name="$1"
    local work_dir="$2"
    shift 2

    echo "Starting ${name}"
    (
        cd "${work_dir}"
        exec "$@"
    ) &
    child_pids+=("$!")
}

stop_children()
{
    local pid

    trap - SIGINT SIGTERM
    if ((${#child_pids[@]} == 0)); then
        return
    fi

    echo "Stopping Lead server processes"
    kill -TERM "${child_pids[@]}" 2>/dev/null || true

    for pid in "${child_pids[@]}"; do
        wait "${pid}" 2>/dev/null || true
    done
}

handle_shutdown()
{
    stop_children
    exit 0
}

trap handle_shutdown SIGINT SIGTERM

db_is_listening()
{
    local local_address
    local remote_address
    local socket_state
    local remainder

    while read -r remainder local_address remote_address socket_state remainder; do
        if [[ "${local_address##*:}" == "3A98" && "${socket_state}" == "0A" ]]; then
            return 0
        fi
    done < /proc/net/tcp

    return 1
}

prepare_db
prepare_game auth auth
prepare_game game1 channel1/game1
prepare_game game2 channel1/game2
prepare_game game99 channel99
prepare_game markserver markserver

start_child db "${runtime_root}/db" "${binary_root}/db"

db_ready=0
for _ in {1..60}; do
    if db_is_listening; then
        db_ready=1
        break
    fi

    if ! kill -0 "${child_pids[0]}" 2>/dev/null; then
        echo "DB core exited before it became ready" >&2
        stop_children
        exit 1
    fi
    sleep 1
done

if ((db_ready == 0)); then
    echo "DB core did not become ready within 60 seconds" >&2
    stop_children
    exit 1
fi

start_child auth "${runtime_root}/auth" "${binary_root}/game" -v
start_child game1 "${runtime_root}/game1" "${binary_root}/game" -v
start_child game2 "${runtime_root}/game2" "${binary_root}/game" -v
start_child game99 "${runtime_root}/game99" "${binary_root}/game" -v
start_child markserver "${runtime_root}/markserver" "${binary_root}/game" -v

set +e
wait -n "${child_pids[@]}"
exit_status=$?
set -e

echo "A Lead server process exited with status ${exit_status}" >&2
stop_children
exit "${exit_status}"
