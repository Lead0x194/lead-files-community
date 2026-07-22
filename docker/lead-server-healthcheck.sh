#!/bin/bash
set -Eeuo pipefail

db_count=0
game_count=0

for process_cmdline in /proc/[0-9]*/cmdline; do
    executable=""
    IFS= read -r -d '' executable < "${process_cmdline}" || true

    case "${executable}" in
        /opt/lead/bin/db)
            ((db_count += 1))
            ;;
        /opt/lead/bin/game)
            ((game_count += 1))
            ;;
    esac
done

[[ "${db_count}" -eq 1 && "${game_count}" -eq 5 ]]
