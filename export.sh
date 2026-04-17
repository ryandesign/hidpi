#!/bin/bash

# SPDX-FileCopyrightText: © 2025 Ryan Carsten Schmidt <https://github.com/ryandesign>
# SPDX-License-Identifier: MIT

# Export files onto an HFS disk image.

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
    [ -n "${tmpfile-}" ] && rm -f "$tmpfile"
    [ -n "${tmpfile2-}" ] && rm -f "$tmpfile2"
    printf "Unmount %s\n" "$dsk"
    humount
}

trap cleanup EXIT

if [ -f "$dsk" ]; then
    printf "Mount %s\n" "$dsk"
    hmount "$dsk" || exit $?
else
    printf "Create %s\n" "$dsk"
    dd if=/dev/zero of="$dsk" bs=1k count=800 status=none || exit $?
    hformat -l "$proj" "$dsk" || exit $?
fi

export_files() {
    local method
    local infile
    local type
    local creator
    local outfile
    while [ $# -gt 0 ]; do
        case $1 in
            -*)
                method="${1:1}"
                type="$2"
                creator="$3"
                shift 2
                ;;
            *)
                infile="$1"
                outfile=$(printf %s "$infile" | iconv -f utf-8 -t macroman)
                printf "Export %s\n" "$infile"
                tmpfile=$(mktemp "$TMPDIR/export.XXXXXXXX")
                case $method in
                    rez)
                        tmpfile2=$(mktemp "$TMPDIR/export.XXXXXXXX")
                        iconv -f utf-8 -t macroman < "$infile.r" > "$tmpfile2" \
                            || return $?
                        Rez "$SDK/RIncludes/Types.r" "$tmpfile2" \
                            -d ALRT_RezTemplateVersion=0 \
                            -d DLOG_RezTemplateVersion=0 \
                            -o "$tmpfile" || return $?
                        rm -f "$tmpfile2"
                        tmpfile2=
                        ;;
                    text)
                        tr '\n' '\r' < "$infile" \
                            | iconv -f utf-8 -t macroman > "$tmpfile" \
                            || return $?
                        ;;
                esac
                SetFile -t "$type" -c "$creator" "$tmpfile" || return $?
                macbinary encode --pipe "$tmpfile" | hcopy -m - ":$outfile" \
                    || return $?
                rm -f "$tmpfile"
                tmpfile=
                ;;
        esac
        shift
    done
}

export_files \
    -rez PROJ KAHL \
        "$proj.π" \
    -rez rsrc RSED \
        "$proj.π.rsrc" \
    -text TEXT KAHL \
        "$proj.c" \
    || exit $?
