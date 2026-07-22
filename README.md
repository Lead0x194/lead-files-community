# Lead Files Community

Server sources and files for Linux/Docker or native FreeBSD. Database setup and
server configuration must be adjusted for your environment.

## Linux with Docker

Requires Docker Engine with Compose v2.

```sh
cp .env.example .env
# Set LEAD_PUBLIC_IP in .env when clients connect remotely.
docker compose -f compose.linux.yml up --build -d
```

```sh
docker compose -f compose.linux.yml logs -f server
docker compose -f compose.linux.yml down
```

The final command retains the MariaDB volume.

## Native FreeBSD

Install the libraries referenced by the existing Makefiles, then build with:

```sh
make -C Lead-Server-Source -j"$(sysctl -n hw.ncpu)"
install -m 755 Lead-Server-Source/game/game Lead-Serverfiles/share/bin/game
install -m 755 Lead-Server-Source/db/db Lead-Serverfiles/share/bin/db
```

After configuring MariaDB and the files under `Lead-Serverfiles`:

```sh
cd Lead-Serverfiles
sh install.sh
sh start.sh
```
