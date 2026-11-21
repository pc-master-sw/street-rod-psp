#!/usr/bin/env bash
set -euo pipefail

root=$(cd "$(dirname "$0")/.." && pwd)
if [[ -n ${PSPDEV:-} ]]; then
    export PATH="$PSPDEV/bin:$PSPDEV/psp/bin:$PATH"
fi
if ! command -v psp-config >/dev/null || ! command -v psp-gcc >/dev/null; then
    echo 'PSPDEV is required. Set PSPDEV or add its tools to PATH.' >&2
    exit 1
fi
if [[ $root =~ [[:space:]] ]]; then
    echo 'Place the checkout in a path without spaces for the PSPSDK makefiles.' >&2
    exit 1
fi

mkdir -p "$root/build/psp"
build=$(mktemp -d "$root/build/psp/street-rod-XXXXXX")
python3 "$root/scripts/prepare_street_rod_engine.py" "$build/engine"
make -f "$root/runtime/psp/psp.mk" -C "$build" PROJECT_ROOT="$root" -j"${JOBS:-2}" 2>&1 | tee "$build/build.log"
psp-gcc --version > "$build/toolchain.txt"
(cd "$build" && sha256sum EBOOT.PBP street_rod.prx > SHA256SUMS.txt)
printf '%s\n' "$build" > "$root/build/psp/latest.txt"
printf 'EBOOT: %s/EBOOT.PBP\n' "$build"
