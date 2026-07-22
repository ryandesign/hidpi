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
    dd if=/dev/zero of="$dsk" bs=1k count=1440 status=none || exit $?
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
                            -d WIND_RezTemplateVersion=0 \
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
        cursor-try.π \
        hidpi.π \
        init.π \
        patches.π \
    -rez rsrc RSED \
        hidpi.π.rsrc \
    -text TEXT KAHL \
        app_data.c \
        app_data.h \
        CloseRgn_patch.c \
        CloseRgn_patch.h \
        constants.h \
        CopyRgn_patch.c \
        CopyRgn_patch.h \
        cursor_stuff.c \
        cursor_stuff.h \
        cursor-try.c \
        debigulate.c \
        debigulate.h \
        debug.c \
        debug.h \
        DiffSectUnionXorRgn_patch.c \
        DiffSectUnionXorRgn_patch.h \
        DisposeHandle_patch.c \
        DisposeHandle_patch.h \
        DisposeRgn_patch.c \
        DisposeRgn_patch.h \
        embiggen.c \
        embiggen.h \
        ExitToShell_patch.c \
        ExitToShell_patch.h \
        FillRect_patch.c \
        FillRect_patch.h \
        globals.c \
        globals.h \
        hidpi.c \
        init_data.c \
        init_data.h \
        init.c \
        InitZone_patch.c \
        InitZone_patch.h \
        InsetOffsetRgn_patch.c \
        InsetOffsetRgn_patch.h \
        install.c \
        install.h \
        JCrsrObscure_patch.c \
        JCrsrObscure_patch.h \
        JHideCursor_patch.c \
        JHideCursor_patch.h \
        JInitCrsr_patch.c \
        JInitCrsr_patch.h \
        JScrnSize_patch.c \
        JScrnSize_patch.h \
        JSetCrsr_patch.c \
        JSetCrsr_patch.h \
        JShieldCursor_patch.c \
        JShieldCursor_patch.h \
        JShowCursor_patch.c \
        JShowCursor_patch.h \
        macros.h \
        missing_traps.h \
        OpenRgn_patch.c \
        OpenRgn_patch.h \
        patch_table.c \
        patch_table.h \
        qdprocs.c \
        qdprocs.h \
        RectRgn_patch.c \
        RectRgn_patch.h \
        rgnset.c \
        rgnset.h \
        ScrnBitMap_patch.c \
        ScrnBitMap_patch.h \
        ScrollRect_patch.c \
        ScrollRect_patch.h \
        SetEmptyRgn_patch.c \
        SetEmptyRgn_patch.h \
        SetRectRgn_patch.c \
        SetRectRgn_patch.h \
        system_requirements.c \
        system_requirements.h \
        SystemTask_patch.c \
        SystemTask_patch.h \
        typedefs.h \
        uninstall.c \
        uninstall.h \
    || exit $?
