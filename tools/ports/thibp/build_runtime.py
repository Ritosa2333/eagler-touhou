"""Build the import-only THIBP Web Runtime from the pinned ph3-modern source."""

import argparse
import json
import os
import subprocess
import sys
from pathlib import Path
from tempfile import TemporaryDirectory
from zipfile import ZipFile

from prepare_launcher import adapt_shell, identity


REVISION = "12ac7141b5cdcd9cabd4479ef76e6453f19fcdf3"
ARCHIVE_SHA256 = "3d5720fcd93f48b6819622b33d72837a99b6ceb170f2bff0029764066033883b"
ADAPTER_ROOT = Path(__file__).resolve().parent
BUILD_LABEL = "kizousen-hud-fps"


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("engine_archive", type=Path)
    parser.add_argument("runtime_output", type=Path)
    args = parser.parse_args()
    output = args.runtime_output.resolve()
    if output.exists():
        parser.error("Runtime output must be a new directory")
    if identity(args.engine_archive)["sha256"] != ARCHIVE_SHA256:
        parser.error(f"Download the pinned ph3-modern ZIP for {REVISION}")

    output.parent.mkdir(parents=True, exist_ok=True)
    with TemporaryDirectory(prefix="thibp-engine-", dir=output.parent) as temp:
        with ZipFile(args.engine_archive) as source:
            source.extractall(temp)
        engine = Path(temp) / f"ph3-modern-{REVISION}"
        subprocess.run(
            ["git", "apply", "--whitespace=nowarn", str(ADAPTER_ROOT / "engine.patch")],
            cwd=engine, check=True,
        )
        build_environment = os.environ.copy()
        build_environment.setdefault("PH3_PYTHON", sys.executable)
        build_environment.setdefault("PH3_EXTRA_FLAGS", json.dumps(["-sWASM_LEGACY_EXCEPTIONS=0"]))
        subprocess.run(
            ["node", "tools/build-web.mjs", "--exceptions=wasm", f"--label={BUILD_LABEL}"],
            cwd=engine, env=build_environment, check=True,
        )
        adapt_shell(engine / f"build/web-wasm-exceptions-{BUILD_LABEL}", output)
    print(f"THIBP Runtime: {output}")


if __name__ == "__main__":
    main()
