"""Read-only native observation of the original5277f0 varargs mismatch."""
import argparse
import hashlib
import json
import pathlib
import subprocess

p = argparse.ArgumentParser()
p.add_argument("original", type=pathlib.Path)
args = p.parse_args()
root = pathlib.Path(__file__).resolve().parent
exe = root / "draw_oracle/build/Release/th20_title_draw_compare.exe"
folder = root / "save_format_observations"
folder.mkdir(exist_ok=True)
rows = []
for bits in [0, 0x3E800000, *range(0x3F800000, 0x3F800008), 0x3DCCCCCD, 0x3A83126F]:
    output = folder / f"{bits:08x}.txt"
    command = [str(exe), str(args.original), str(output), f"{bits:08x}"]
    try:
        result = subprocess.run(command, capture_output=True, timeout=15,
                                creationflags=subprocess.CREATE_NO_WINDOW)
        rows.append({"bits": f"{bits:08x}", "exit_code": result.returncode,
                     "stdout": result.stdout.decode("utf-8", "replace"),
                     "stderr": result.stderr.decode("utf-8", "replace"),
                     "output": output.relative_to(root).as_posix(),
                     "text": output.read_text(errors="replace") if output.exists() else None})
    except subprocess.TimeoutExpired:
        rows.append({"bits": f"{bits:08x}", "status": "timeout"})
report = {"original_sha256": hashlib.sha256(args.original.read_bytes()).hexdigest(),
          "probe_exe_sha256": hashlib.sha256(exe.read_bytes()).hexdigest(),
          "entry": "005277f0", "phase": 2,
          "source": "draw_oracle/compare.cpp native-only argument",
          "observations": rows}
(root / "save_format_observations.json").write_text(json.dumps(report, indent=2))
for row in rows:
    print(row["bits"], row.get("exit_code", row.get("status")),
          (row.get("text") or "").splitlines()[2:4])
