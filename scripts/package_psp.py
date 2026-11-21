"""Package the native EBOOT and documentation for PSP installation."""

import argparse
import hashlib
from pathlib import Path
from zipfile import ZIP_DEFLATED, ZipFile


def package(eboot: Path, archive: Path) -> None:
    root = Path(__file__).resolve().parents[1]
    binary = eboot.read_bytes()
    if not binary.startswith(b"\x00PBP"):
        raise ValueError(f"Invalid PBP file: {eboot}")
    digest = hashlib.sha256(binary).hexdigest()

    documents = {
        "LICENSE": "COPYING.DOS2PSP.txt",
        "vendor/street-rod-sdl3/LICENSE": "LICENSE.street-rod-sdl3.txt",
        "docs/INSTALACION.md": "INSTALACION.md",
        "docs/USO.md": "USO.md",
    }
    archive.parent.mkdir(parents=True, exist_ok=True)
    with ZipFile(archive, "x", ZIP_DEFLATED) as output:
        output.writestr("STREETROD/EBOOT.PBP", binary)
        output.writestr("STREETROD/SHA256SUMS.txt", f"{digest}  EBOOT.PBP\n")
        output.writestr(
            "STREETROD/Game/ARCHIVOS.txt",
            "Coloca aqui SR.EXE, LIB1, LIB2 y HOT_DATA de Street Rod 1.\n",
        )
        for source_name, target_name in documents.items():
            output.write(root / source_name, f"STREETROD/{target_name}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("eboot", type=Path)
    parser.add_argument("archive", type=Path)
    arguments = parser.parse_args()
    package(arguments.eboot, arguments.archive)
