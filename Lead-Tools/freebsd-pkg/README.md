# Lead server — FreeBSD package

This directory builds a native FreeBSD/amd64 package, **`lead-server`**, that
turns a bare FreeBSD host into a running Lead server with a single command:

```sh
pkg install lead-server
```

That one command:

- pulls in **MariaDB** and **lzo2** automatically (declared dependencies,
  resolved from the official FreeBSD repo);
- installs the `db` + `game` cores, the shared content tree, the per-core
  configs, the DB-bootstrap tool and an `rc.d` service;
- creates the `lead` user, builds the runtime tree and **symlinks** under
  `/var/db/lead` (replacing every old `install.sh`);
- starts MariaDB and **creates the databases, the SQL user and applies every
  migration** (idempotently);
- enables the `lead` service.

The package is published as a **pkg repository on GitHub Pages**, so new
releases are delivered with `pkg upgrade` — and each upgrade re-applies only the
new database migrations.

---

## Layout once installed

| Path | Contents |
|------|----------|
| `/usr/local/libexec/lead/{game,db}` | the two server binaries |
| `/usr/local/libexec/lead/lead-db-setup` | idempotent DB bootstrap |
| `/usr/local/libexec/lead/lead-layout` | (re)builds the runtime tree + symlinks |
| `/usr/local/share/lead/{data,locale,package,conf}` | static content |
| `/usr/local/share/lead/db-scripts/{base,migrations}` | SQL |
| `/usr/local/etc/lead/<core>/CONFIG`, `/usr/local/etc/lead/db/conf.txt` | configs (`@config`) |
| `/usr/local/etc/rc.d/lead` | service |
| `/var/db/lead/<core>/` | per-core working dirs (logs, cores, marks) + symlinks |

Cores shipped: `db`, `auth`, `channel1/game1`, `channel1/game2`, `channel99`,
`markserver`. Adjust the running set via `lead_cores` in `/etc/rc.conf`.

---

## For the server admin

1. **Add the repo** (once):
   ```sh
   fetch -o /usr/local/etc/pkg/repos/lead.conf \
       https://INDEX-S.github.io/lead-files-community/lead.conf
   pkg update
   ```
2. **Install**:
   ```sh
   pkg install lead-server
   ```
3. **Set the public/bind IP** in the `@config` files before first start:
   `/usr/local/etc/lead/auth/CONFIG`, the `channel*/CONFIG`s and
   `/usr/local/etc/lead/db/conf.txt`.
4. **Start**:
   ```sh
   service lead start
   service lead status
   ```
5. **Upgrade later**:
   ```sh
   pkg update && pkg upgrade lead-server   # new migrations apply automatically
   ```

Re-run the DB bootstrap any time (safe, idempotent):
```sh
/usr/local/libexec/lead/lead-db-setup
```

If your database is **not** local (e.g. a shared MariaDB), point the tool at it:
```sh
LEAD_DB_HOST=10.0.0.5 LEAD_DB_ADMIN_USER=root LEAD_DB_ADMIN_PASSWORD=secret \
    /usr/local/libexec/lead/lead-db-setup
```

---

## For the maintainer (building the package)

Everything must be built **on FreeBSD/amd64** (it produces amd64 ELF binaries).

### Manual build (your QEMU VM)

```sh
# one-time: install build deps (needs root)
sh Lead-Tools/freebsd-pkg/build.sh --install-deps --no-build

# build the package
sh Lead-Tools/freebsd-pkg/build.sh --version 0.1.0
# -> Lead-Tools/freebsd-pkg/dist/lead-server-0.1.0.pkg

# turn it into a publishable repo (optionally signed)
sh Lead-Tools/freebsd-pkg/make-repo.sh --url https://INDEX-S.github.io/lead-files-community
# -> Lead-Tools/freebsd-pkg/repo-out/   (publish this tree to gh-pages)
```

Test it on a clean jail/VM:
```sh
pkg add ./Lead-Tools/freebsd-pkg/dist/lead-server-0.1.0.pkg   # or via the repo
service lead start
```

### Automated build (GitHub Actions)

`.github/workflows/freebsd-pkg.yml` builds in a FreeBSD VM and publishes when you
**publish a GitHub Release** tagged `vX.Y.Z` (same flow as upstream):

```sh
gh release create v0.1.0 --title "v0.1.0" --notes "..."   # or via the web UI
```

- Repo is published to the **gh-pages** branch (enable Pages → branch `gh-pages`).
- The `.pkg` is also attached to that release.
- `workflow_dispatch` is available for manual test builds (no release needed).
- Optional signing: add an RSA private key as the `LEAD_REPO_SIGNING_KEY` secret;
  the workflow signs the catalog and emits `lead-repo.pub` (clients install it to
  `/usr/local/etc/pkg/keys/` and set `signature_type: "pubkey"`).

### Bumping the FreeBSD version (when a newer release appears)

The FreeBSD version is a single knob in each place that needs it — no other files
reference it (build.sh derives the package ABI from the build host automatically):

| Where | How to bump |
|-------|-------------|
| **GitHub Actions** | Set repo variable **`FREEBSD_RELEASE`** (Settings → Secrets and variables → Actions → Variables), **or** edit the `FREEBSD_RELEASE` default in `.github/workflows/freebsd-pkg.yml`, **or** pass `freebsd_release` to a manual `workflow_dispatch` run. |
| **Local Vagrant build** | Set env var **`LEAD_FREEBSD_BOX`** (e.g. `generic/freebsd16`) or edit the default in `Vagrantfile`. |
| **Manual `build.sh` on a VM** | Nothing — just run it on whatever FreeBSD version the host is; the package is tagged for that release. |

So a new FreeBSD release = change one variable and re-run; the next published
Release builds and ships a package for it.

---

## How the pieces map to your old workflow

| Old | New |
|-----|-----|
| `Lead-Serverfiles/*/install.sh` (symlinks, dirs, chmod) | `lead-layout` + plist (`@dir`/`@config`) |
| `Lead-Serverfiles/start.sh`, per-core `run.sh` | `service lead start` (`rc.d/lead`) |
| `Lead-Database-Scripts/setup.py` | `lead-db-setup` (idempotent, migration-tracked, latin1-safe) |
| manual dep install | declared `deps` (`pkg` pulls MariaDB + lzo2) |
| copy binaries by hand | `pkg install` / `pkg upgrade` |

### Notes / trade-offs

- The post-install **starts MariaDB and seeds the schema**. This is slightly
  outside official-ports etiquette but intended for a turnkey appliance; every
  step is best-effort so `pkg install` never fails on it, and idempotency makes
  re-runs safe.
- Player data and the databases are **preserved on `pkg delete`** — removal is
  left to the admin (see the deinstall message).
- The server links almost everything statically; the only non-base runtime
  dependency is `lzo2`. `build.sh` prints `ldd` output so the manifest stays
  honest if that ever changes.
