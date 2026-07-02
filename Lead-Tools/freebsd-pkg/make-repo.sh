#!/bin/sh
#
# make-repo.sh - turn a directory of .pkg files into a FreeBSD pkg repository
# that can be published to GitHub Pages (or any static HTTPS host) and consumed
# with `pkg install` / `pkg upgrade`.
#
# Usage:
#   Lead-Tools/freebsd-pkg/make-repo.sh [options]
#     --pkgdir  <dir>   directory containing the .pkg files (default: dist)
#     --repodir <dir>   output repository directory       (default: repo-out)
#     --key     <file>  RSA private key to SIGN the catalog (optional but
#                       recommended for non-HTTPS hosts). Public key is written
#                       next to the repo as lead-repo.pub.
#     --url     <url>   base URL the repo will be served from; used only to
#                       render the sample client config (lead.conf).
#
# Publish the resulting <repodir> tree to gh-pages; clients then point a repo
# config at the URL. See Lead-Tools/freebsd-pkg/repo/lead.conf and README.md.
#
set -eu

SELF="$(realpath "$0")"; PKGDIR="$(dirname "$SELF")"
PKGSRC="${PKGDIR}/dist"
REPODIR="${PKGDIR}/repo-out"
KEY=""
# Derive the GitHub Pages URL from the single repo-identity variable (owner/name).
# Override LEAD_REPO_URL directly, or LEAD_REPO to retarget a fork.
LEAD_REPO="${LEAD_REPO:-Index-s/lead-files-community}"
_owner="$(printf '%s' "${LEAD_REPO%%/*}" | tr 'A-Z' 'a-z')"
_name="${LEAD_REPO##*/}"
URL="${LEAD_REPO_URL:-https://${_owner}.github.io/${_name}}"

while [ $# -gt 0 ]; do
	case "$1" in
		--pkgdir)  PKGSRC="$2"; shift 2 ;;
		--repodir) REPODIR="$2"; shift 2 ;;
		--key)     KEY="$2"; shift 2 ;;
		--url)     URL="$2"; shift 2 ;;
		-h|--help) grep '^#' "$0" | sed 's/^# \{0,1\}//'; exit 0 ;;
		*) echo "make-repo.sh: unknown argument: $1" >&2; exit 2 ;;
	esac
done

log() { echo "==> $*"; }
die() { echo "make-repo.sh: ERROR: $*" >&2; exit 1; }
command -v pkg >/dev/null 2>&1 || die "pkg not found (run on FreeBSD)"

ls "${PKGSRC}"/*.pkg >/dev/null 2>&1 || die "no .pkg files in ${PKGSRC}"

log "assembling repo in ${REPODIR}"
rm -rf "$REPODIR"; mkdir -p "${REPODIR}/All"
cp "${PKGSRC}"/*.pkg "${REPODIR}/All/"

if [ -n "$KEY" ]; then
	[ -f "$KEY" ] || die "signing key not found: $KEY"
	log "generating SIGNED catalog"
	pkg repo "$REPODIR" "$KEY"
	openssl rsa -in "$KEY" -pubout -out "${REPODIR}/lead-repo.pub" 2>/dev/null \
		|| die "could not derive public key from $KEY"
	SIGTYPE="pubkey"
else
	log "generating UNSIGNED catalog (relies on HTTPS transport trust)"
	pkg repo "$REPODIR"
	SIGTYPE="none"
fi

# Render a sample client config matching how this repo was built.
mkdir -p "${PKGDIR}/repo"
if [ "$SIGTYPE" = "pubkey" ]; then
	cat > "${REPODIR}/lead.conf" <<EOF
lead: {
    url: "${URL}",
    enabled: yes,
    signature_type: "pubkey",
    pubkey: "/usr/local/etc/pkg/keys/lead-repo.pub",
}
EOF
else
	cat > "${REPODIR}/lead.conf" <<EOF
lead: {
    url: "${URL}",
    enabled: yes,
    signature_type: "none",
}
EOF
fi

log "repository ready: ${REPODIR}"
log "  catalog : meta.conf, packagesite.pkg, data.pkg"
log "  packages: All/*.pkg"
log "  client  : lead.conf  (install to /usr/local/etc/pkg/repos/lead.conf)"
[ "$SIGTYPE" = "pubkey" ] && log "  pubkey  : lead-repo.pub (install to /usr/local/etc/pkg/keys/ on clients)"
