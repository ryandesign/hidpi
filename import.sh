#!/bin/bash

# SPDX-FileCopyrightText: © 2025 Ryan Carsten Schmidt <https://github.com/ryandesign>
# SPDX-License-Identifier: MIT

# Import files from an HFS disk image.

set -uo pipefail

: "${SDK=/opt/local/libexec/Retro68/universal}"
: "${TMPDIR=/tmp}"

proj=hidpi
dsk="$proj.dsk"

err() {
    local msg="$1"
    local code="${2-1}"

    printf "%s: %s\n" "$(basename "$0")" "$msg" 1>&2
    exit "$code"
}

[ "$(uname -s)" = "Darwin" ] || err "this script only works on macOS"

cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &>/dev/null || exit $?

cleanup() {
    [ -d "${tmpdir-}" ] && rm -rf "$tmpdir"
    printf "Unmount %s\n" "$dsk"
    humount
}

trap cleanup EXIT

printf "Mount %s\n" "$dsk"
hmount "$dsk" || exit $?

# The macbinary tool won't write to a file that exists, so we must make a
# temporary directory for it to write into. Its manpage documents a --force flag
# that can be used to make it write to an existing file, but the tool doesn't
# actually have any such option. Bug report filed with Apple as FB17654870.
tmpdir=$(mktemp -d "$TMPDIR/import.XXXXXXXX")
tmpfile="$tmpdir/tmp"

import_files() {
    local method
    local outfile
    local infile
    local optional=false
    local skip
    while [ $# -gt 0 ]; do
        case $1 in
            -optional)
                optional=true
                ;;
            -*)
                method="${1:1}"
                ;;
            *)
                outfile="$1"
                infile=$(printf %s "$outfile" | iconv -f utf-8 -t macroman)
                skip=false
                if "$optional"; then
                    [ -z "$(hls "$proj" 2>/dev/null)" ] && skip=true
                fi
                if "$skip"; then
                    printf "Skip %s\n" "$outfile"
                else
                    printf "Import %s\n" "$outfile"
                    if [ "$method" = "macbinary" ]; then
                        hcopy -m ":$infile" "$outfile.bin" || return $?
                    else
                        hcopy -m ":$infile" - \
                            | macbinary decode --pipe -o "$tmpfile" || return $?
                        case $method in
                            rez)
                                DeRez "$tmpfile" "$SDK/RIncludes/Types.r" ./types.r \
                                    -d ALRT_RezTemplateVersion=0 \
                                    -d DLOG_RezTemplateVersion=0 \
                                    | LC_CTYPE=C sed -E 's/^(\t\$"[^"]*").*/\1/' \
                                    | iconv -f macroman -t utf-8 \
                                    | ./sortrez.pl > "$outfile.r" || return $?
                                ;;
                            text)
                                iconv -f macroman -t utf-8 < "$tmpfile" \
                                    | tr '\r' '\n' > "$outfile" || return $?
                                ;;
                        esac
                        rm -f "$tmpfile"
                    fi
                fi
                ;;
        esac
        shift
    done
}

import_files \
    -rez \
        "$proj.π" "$proj.π.rsrc" \
    -text \
        "$proj.c" qdprocs.c qdprocs.h \
    -optional \
        -macbinary \
            "$proj" \
    || exit $?
