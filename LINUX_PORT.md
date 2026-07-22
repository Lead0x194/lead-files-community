# Linux server port

## Status

The Lead server now builds and runs on Linux with an explicit `__LINUX__`
compile-time flag. The tested Docker topology starts one DB core and five game
cores (auth, game1, game2, channel99, and markserver) against the repository's
existing MariaDB setup.

This port intentionally changes as little of the original project as possible:

- Existing FreeBSD code paths and linker settings remain the default for the
  Makefile build on non-Linux Unix hosts.
- Existing Windows project files and Windows/select code paths are unchanged.
- Linux-specific behavior is isolated behind `__LINUX__` or `HOST_OS=Linux`.
- Database schemas, base SQL, migrations, setup behavior, and credentials have
  not been redesigned. Database work is explicitly deferred.
- All server processes remain in one container. This preserves the original
  single-host assumptions and avoids changing the internal protocol topology.

No commit was created for this work.

## Platform behavior

| Platform | Compile-time selection | Event backend | Link strategy |
| --- | --- | --- | --- |
| FreeBSD | Compiler-provided `__FreeBSD__` | Existing kqueue implementation | Existing `/usr/local` and static library paths |
| Windows | Existing Visual Studio macros/projects | Existing select implementation | Existing Visual Studio projects |
| Linux | New Makefile-provided `__LINUX__` | New epoll implementation | Ubuntu system development/runtime libraries |

`Lead-Server-Source/platform.mk` detects Linux with `uname -s`, adds
`-D__LINUX__`, and selects C++17 only for the Linux build. It is included by
the six existing Makefiles without changing their FreeBSD defaults.

## Implementation record

### Server source

- Added the Linux epoll fdwatch backend, including read, write, EOF, and
  one-shot write handling. FreeBSD kqueue and Windows/select implementations
  remain separate and unchanged.
- Kept the existing POSIX signal implementation for Linux and included the
  system signal header despite the project's local `signal.h` filename.
- Used Linux `srandom()` initialization while retaining FreeBSD
  `srandomdev()` and the Windows random initialization.
- Used the system `dirent.h` implementation on Linux while retaining the
  bundled Windows directory implementation.
- Avoided the BSD-only `optreset` assignment on Linux. The existing reset
  behavior remains enabled on FreeBSD and Windows.
- Added the missing standard `<type_traits>` include required by the existing
  C++17 shared utility code.
- Added an optional Linux-only `P2P_HOST` DB-core setting. In the Docker
  topology it is `127.0.0.1`, while the game cores can still advertise the
  externally reachable `LEAD_PUBLIC_IP` to clients. The original peer public-IP
  behavior remains the fallback and is unchanged on FreeBSD and Windows.

### Build and runtime packaging

- `Dockerfile.linux` builds only the shared and server sources in an Ubuntu
  24.04 amd64 builder, then copies the two linked binaries and required server
  data into a smaller Ubuntu runtime image.
- The top-level server Makefile declares the library dependencies of `game` and
  `db`. Docker uses `make -j"$(nproc)"`, allowing independent libraries and
  source files to compile concurrently while preventing either executable from
  linking before its required libraries are complete.
- The runtime runs as the unprivileged `lead` user (UID 10001).
- `.dockerignore` excludes the client, tools, database scripts, repository
  history, old binaries, and build artifacts from the server image context.
- `docker/lead-server-entrypoint.sh` renders runtime configuration into
  `/srv/lead`, starts the DB core first, waits for its listener without opening
  a protocol connection, then starts all five game cores. It forwards TERM/INT
  to every child and exits if any child exits unexpectedly.
- `docker/lead-server-healthcheck.sh` verifies that exactly one DB executable
  and five game executables are alive. It reads process command lines so it
  works on native Linux and under Docker Desktop's amd64 emulation without
  creating noise in application socket logs.

### Compose topology

`compose.linux.yml` extends the existing `Lead-Database-Scripts/compose.yml`
database service rather than replacing it. The added setup image only packages
and invokes the existing `setup.py`; the script and SQL content are unchanged.

```text
MariaDB 10.6.27 -> existing setup.py -> Linux server container
                                      |- DB core
                                      |- auth
                                      |- game1
                                      |- game2
                                      |- channel99
                                      `- markserver
```

The server cores use localhost for DB-core and P2P communication inside their
container. MariaDB is reached by the Compose service name `db-lead`.

## Running the server

Docker Engine with the Compose v2 plugin is required. The image currently
targets `linux/amd64`; Docker Desktop can run it with emulation on an ARM host.

For a direct Linux source build outside Docker, use:

```sh
make -C Lead-Server-Source -j"$(nproc)"
```

1. Create the local environment file:

   ```sh
   cp .env.example .env
   ```

2. If clients connect from another machine, change `LEAD_PUBLIC_IP` in `.env`
   to the IPv4 address that clients can reach. Keep `127.0.0.1` for a client on
   the Docker host.

3. Build and start:

   ```sh
   docker compose -f compose.linux.yml up --build -d
   ```

4. Inspect health and logs:

   ```sh
   docker compose -f compose.linux.yml ps
   docker compose -f compose.linux.yml logs -f server
   ```

5. Stop the stack without deleting database data:

   ```sh
   docker compose -f compose.linux.yml down
   ```

Do not add `--volumes` to the final command unless permanent deletion of the
MariaDB volume is intentional.

## Published ports

| Port | Purpose |
| ---: | --- |
| 11000 | Authentication service |
| 13000 | Mark server |
| 13001 | Channel 1, game core 1 |
| 13002 | Channel 1, game core 2 |
| 13099 | Channel 99 |
| 3306 | Existing MariaDB service |

The DB-core and P2P ports (12000, 14000-14002, 14099, and 15000) remain internal
to the server container.

## Validation performed

The following checks passed on 2026-07-21; the parallel source build was
revalidated on 2026-07-22:

- Full clean compilation and linking of both `game/game` and `db/db` in the
  Ubuntu 24.04 `linux/amd64` builder, including the parallel dependency graph.
- Runtime shared-library resolution for both binaries.
- `docker compose -f compose.linux.yml config`.
- Shell syntax checks for both runtime scripts.
- `git diff --check`.
- MariaDB health and successful completion of the existing database setup.
- Server container health with exactly one DB core and five game cores.
- Internal listeners on 11000, 12000, 13000, 13001, 13002, 13099, 14000,
  14001, 14002, 14099, and 15000.
- DB-core SETUP registration from the auth, game, channel99, and mark cores.
- P2P connectors and acceptors using `127.0.0.1`.
- A Compose restart: signals reached the child processes, the service returned
  to healthy, and the P2P connections registered again.

At handoff, the smoke-test stack is intentionally still running. It uses the
named volume `lead-linux_lead-dbdata`, created during this validation.

## Deferred work and known limitations

- The database is intentionally unchanged beyond packaging its existing setup
  command. Its root credentials, exposed port, schema setup, migration policy,
  and production hardening need a separate database-focused pass.
- The existing setup script's behavior for empty or already-populated databases
  is unchanged. This port does not add a migration runner.
- Only `linux/amd64` has been validated. ARM64-native compilation is not part of
  this pass.
- Client login/gameplay was not exercised end-to-end. The server-side listener,
  database, and P2P lifecycle were exercised.
- Existing server data reports map-41 sectree/guild-NPC warnings in game2,
  channel99, and markserver logs. The affected processes remain running; these
  are pre-existing data/configuration issues, not Linux build or event-loop
  failures.
- Existing compiler warnings in legacy logging and Lua code remain. They do not
  block the Linux build.
- Splitting each core into its own container would require a broader change to
  public/client addressing versus internal P2P addressing. The single server
  container is deliberate for this minimal port.

## Review and rollback

Review the uncommitted work with:

```sh
git status --short
git diff --check
git diff
```

To stop the running validation environment while retaining the database, run:

```sh
docker compose -f compose.linux.yml down
```

No source-control commit or push was performed.
