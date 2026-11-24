"""Prepare SoEW Native runtime assets from the user's own PC-98 HDI."""
import argparse
from pathlib import Path
import tempfile

from hdi_baseline import inspect
from pf_extract import extract


def prepare(image: Path, output: Path) -> int:
    if output.exists() or output.is_symlink():
        raise ValueError(f"Choose a new content directory: {output}")
    if not output.parent.is_dir():
        raise ValueError(f"Output parent must already exist: {output.parent}")
    source = image.read_bytes()
    files = {Path(row["path"]) for row in inspect(source)["files"]}
    archives = sorted(path for path in files if path.name == "東方封魔.録")
    if len(archives) != 1:
        raise ValueError("Expected exactly one 東方封魔.録 in the TH02 disk image")
    archive = archives[0]
    scores = archive.with_name("HUUHI.DAT")
    selected = {archive, scores} & files
    payloads = inspect(source, select=selected)
    with tempfile.TemporaryDirectory(prefix=".soew-assets-", dir=output.parent) as temporary:
        root = Path(temporary)
        local_archive = root / "input.pf"
        local_archive.write_bytes(payloads[archive])
        content = root / "content"
        entries = extract(local_archive, content, allow_size_mismatch=True)
        for entry in entries:
            if entry["size_matches"]:
                continue
            if (entry["name"], entry["size"], entry["decoded_size"]) != ("MIKO16.BFT", 10320, 10321):
                raise ValueError(f"Unexpected decoded size for {entry['name']}")
        required = {"OP.PI", "OP2.PI", "OP.RGB", "MIKOFT.BFT", "HUUMA.EFC"}
        required.update(f"STAGE{i}.{ext}" for i in range(6) for ext in ("DT1", "MPN", "MAP"))
        missing = sorted(name for name in required if not (content / name).is_file())
        if missing:
            raise ValueError("Missing game assets: " + ", ".join(missing))
        if scores in payloads and not (content / scores.name).exists():
            (content / scores.name).write_bytes(payloads[scores])
        content.rename(output)
    return len(entries)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("image", type=Path, help="Your own TH02 HDI image")
    parser.add_argument("output", type=Path, help="New runtime content directory")
    args = parser.parse_args()
    try:
        count = prepare(args.image, args.output)
    except (OSError, ValueError) as error:
        parser.exit(1, f"Asset preparation failed: {error}\n")
    print(f"Prepared {count} archived assets in {args.output}")


if __name__ == "__main__":
    main()
