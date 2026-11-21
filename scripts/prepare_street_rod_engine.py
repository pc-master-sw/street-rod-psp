"""Materialize the pinned Street Rod engine into a build directory."""

import argparse
from pathlib import Path
import shutil


def prepare(root, destination):
    upstream = root / "vendor/street-rod-sdl3"
    upstream_src = upstream / "srport/src"
    destination.mkdir(parents=True, exist_ok=True)
    shutil.copyfile(upstream / "LICENSE", destination / "LICENSE")
    for source in upstream_src.rglob("*"):
        if not source.is_file() or source in (upstream_src / "main.c", upstream_src / "host.c"):
            continue
        output = destination / source.relative_to(upstream)
        output.parent.mkdir(parents=True, exist_ok=True)
        shutil.copyfile(source, output)

    src = destination / "srport/src"
    types = src / "types.h"
    source = types.read_text(encoding="utf-8")
    for name in ("u16", "s16", "u32", "s32"):
        source = source.replace(f" {name};", f" {name} __attribute__((aligned(1), may_alias));")
    types.write_text(source, encoding="utf-8")

    loader = src / "mem.c"
    source = loader.read_text(encoding="utf-8").replace(
        "#include <SDL3/SDL.h>", '#include "engine_adapter.h"\n#include <errno.h>\n#include <stdlib.h>'
    )
    for previous, replacement in (
        ("SDL_LoadFile", "sr_load_file"), ("SDL_GetError()", "strerror(errno)"),
        ("SDL_malloc", "malloc"), ("SDL_calloc", "calloc"),
        ("SDL_realloc", "realloc"), ("SDL_free", "free"),
    ):
        source = source.replace(previous, replacement)
    if "SDL_" in source:
        raise ValueError("unexpected SDL call in the pinned executable loader")
    source = source.replace(
        "u16 n = le16(stub + p); p += 2;",
        "u16 n = le16(stub + p); p += 2;\n"
        "        if (n > (stub_len - p) / 2) { set_err(err, errlen, \"EXEPACK relocations truncated\"); return false; }"
    ).replace(
        "size_t packed_len = ep - (size_t)(skip_len - 1) * 16;",
        "if (!skip_len || (size_t)(skip_len - 1) > ep / 16) {\n"
        "        set_err(err, errlen, \"invalid EXEPACK skip length\"); return false;\n"
        "    }\n"
        "    size_t packed_len = ep - (size_t)(skip_len - 1) * 16;"
    ).replace(
        "size_t mz_len = (size_t)cp * 512",
        "if (!cp || cblp >= 512) { set_err(err, errlen, \"invalid MZ file size\"); goto done; }\n"
        "    size_t mz_len = (size_t)cp * 512"
    ).replace(
        "relocs = malloc(sizeof(Reloc) * (crlc + 1u));",
        "relocs = malloc(sizeof(Reloc) * (crlc + 1u));\n"
        "        if (!relocs) { set_err(err, errlen, \"out of memory\"); goto done; }"
    )
    loader.write_text(source, encoding="utf-8")

    dos = src / "platform/dos.c"
    source = dos.read_text(encoding="utf-8")
    source = '#include "engine_adapter.h"\n' + source.replace(
        "if (!*base) return 2;",
        "if (!*base) return 2;\n    if (fmode[0] != 'r' || strchr(fmode, '+')) {\n"
        "        if (!sr_writable_file(base)) return 5;\n    }",
        1
    ).replace(
        "char *path = *base ? host_game_path(base, false) : NULL;",
        "char *path = sr_writable_file(base) ? host_game_path(base, false) : NULL;"
    )
    dos.write_text(source, encoding="utf-8")

    keyboard = src / "platform/kbd.c"
    source = keyboard.read_text(encoding="utf-8")
    source = '#include "engine_adapter.h"\n' + source.replace(
        "if (DSB(DS_racing) != 0 && (sc ==",
        "if (sr_is_driving() && DSB(DS_racing) != 0 && (sc =="
    )
    keyboard.write_text(source, encoding="utf-8")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("destination", type=Path)
    args = parser.parse_args()
    prepare(Path(__file__).resolve().parents[1], args.destination)
