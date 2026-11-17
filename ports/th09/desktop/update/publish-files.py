import argparse
import gzip
import hashlib
import json
from pathlib import Path
import re
import shutil
from tempfile import NamedTemporaryFile


ALLOWED_FILE = re.compile(
    r"(eagler-desktop\.exe|catalog\.template\.json|runtime/[A-Za-z0-9][A-Za-z0-9._-]*\.(exe|dll|ttf|bmp|txt)"
    r"|runtime/fonts/(blend|cp932)\.bin|licenses/[A-Za-z0-9][A-Za-z0-9._-]*\.txt)"
)
REQUIRED = {
    "eagler-desktop.exe", "catalog.template.json", "runtime/th09.exe",
    "runtime/ChillRoundGothic_Medium.ttf", "runtime/fonts/cp932.bin", "runtime/fonts/blend.bin",
}


def publish(source, output):
    if output == source or source in output.parents:
        raise ValueError("The update feed must be outside the runtime directory")
    files = {
        path.relative_to(source).as_posix(): path
        for section in ("runtime", "runtime/fonts", "licenses")
        if (source / section).is_dir()
        for path in (source / section).iterdir()
        if path.is_file() and ALLOWED_FILE.fullmatch(path.relative_to(source).as_posix())
    }
    files.update({name: source / name for name in ("eagler-desktop.exe", "catalog.template.json") if (source / name).is_file()})
    if not REQUIRED <= files.keys() or len(files) > 128:
        raise ValueError("Expected a Windows th09 runtime with the required executables and font tables")

    objects = output / "objects"
    objects.mkdir(parents=True, exist_ok=True)
    entries = []
    for name, path in sorted(files.items()):
        if path.resolve() != path or path.stat().st_size not in range(1, 1_000_000_000):
            raise ValueError("Invalid update source file: " + name)
        with path.open("rb") as stream:
            digest = hashlib.file_digest(stream, "sha256").hexdigest()
        blob = objects / (digest + ".gz")
        if blob.exists():
            with gzip.open(blob, "rb") as stream:
                if hashlib.file_digest(stream, "sha256").hexdigest() != digest:
                    raise ValueError("Corrupt existing update object: " + blob.name)
        else:
            with NamedTemporaryFile(dir=objects, delete=False) as staging:
                temporary = Path(staging.name)
            try:
                with path.open("rb") as source_file, temporary.open("wb") as output_file:
                    with gzip.GzipFile(filename="", fileobj=output_file, mode="wb", compresslevel=6, mtime=0) as compressed:
                        shutil.copyfileobj(source_file, compressed, 131072)
                with gzip.open(temporary, "rb") as stream:
                    if hashlib.file_digest(stream, "sha256").hexdigest() != digest:
                        raise ValueError("Source changed during publication: " + name)
                temporary.replace(blob)
            finally:
                temporary.unlink(missing_ok=True)
        entries.append({"path": name, "sha256": digest, "size": path.stat().st_size, "downloadSize": blob.stat().st_size})

    manifest = {"schema": "eagler-th09-windows-update/2", "files": entries}
    with NamedTemporaryFile(dir=output, delete=False) as staging:
        temporary = Path(staging.name)
        staging.write((json.dumps(manifest, separators=(",", ":")) + "\n").encode("ascii"))
    try:
        temporary.replace(output / "manifest.json")
    finally:
        temporary.unlink(missing_ok=True)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Publish individual update files from an existing th09 Windows runtime.")
    parser.add_argument("runtime", type=Path)
    parser.add_argument("output", type=Path)
    args = parser.parse_args()
    publish(args.runtime.resolve(), args.output.resolve())
