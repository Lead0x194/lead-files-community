import os
import argparse
import pymysql

# Defaults
DEFAULT_HOST = "127.0.0.1"
DEFAULT_PORT = 3306
DEFAULT_USER = "root"
DEFAULT_PASSWORD = "password"

DATABASES = ["account", "common", "log", "player"]


def parse_args():
    parser = argparse.ArgumentParser(description="Lead Files DB Setup")

    parser.add_argument("--host", default=DEFAULT_HOST)
    parser.add_argument("--port", type=int, default=DEFAULT_PORT)
    parser.add_argument("--user", default=DEFAULT_USER)
    parser.add_argument("--password", default=DEFAULT_PASSWORD)

    return parser.parse_args()


def get_paths():
    base_dir = os.path.dirname(os.path.abspath(__file__))

    base_path = os.path.join(base_dir, "base")
    migrations_path = os.path.join(base_dir, "migrations")

    return base_path, migrations_path


def get_connection(host, port, user, password, db=None):
    return pymysql.connect(
        host=host,
        port=port,
        user=user,
        password=password,
        database=db,
        autocommit=True,
        charset="utf8mb4",
        client_flag=pymysql.constants.CLIENT.MULTI_STATEMENTS
    )


def check_existing_databases(conn):
    cursor = conn.cursor()

    existing = []
    for db in DATABASES:
        cursor.execute("SHOW DATABASES LIKE %s;", (db,))
        if cursor.fetchone():
            existing.append(db)

    if existing:
        print(f"❌ Datenbanken existieren bereits: {', '.join(existing)}. Abbruch.")
        return True

    return False


def create_databases(conn):
    cursor = conn.cursor()

    for db in DATABASES:
        print(f"→ Erstelle Datenbank: {db}")
        cursor.execute(f"CREATE DATABASE {db} CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci;")


def execute_sql_file(conn_params, db, filepath):
    print(f"→ [{db or 'migrations'}] {os.path.basename(filepath)}")

    with open(filepath, "r", encoding="utf-8") as f:
        sql = f.read()

    conn = get_connection(**conn_params, db=db)
    cursor = conn.cursor()

    cursor.execute(sql)
    while cursor.nextset():
        pass

    conn.close()


def run_base(conn_params, base_path):
    for db in DATABASES:
        file = os.path.join(base_path, f"{db}.sql")
        execute_sql_file(conn_params, db, file)


def run_migrations(conn_params, migrations_path):
    files = sorted(os.listdir(migrations_path))

    for file in files:
        if not file.endswith(".sql"):
            continue

        filepath = os.path.join(migrations_path, file)

        execute_sql_file(conn_params, None, filepath)


def main():
    args = parse_args()

    conn_params = {
        "host": args.host,
        "port": args.port,
        "user": args.user,
        "password": args.password
    }

    base_path, migrations_path = get_paths()

    conn = get_connection(**conn_params)

    if check_existing_databases(conn):
        conn.close()
        return

    print("→ Starte Setup...")

    create_databases(conn)
    conn.close()

    run_base(conn_params, base_path)
    run_migrations(conn_params, migrations_path)

    print("✅ Setup abgeschlossen.")


if __name__ == "__main__":
    main()