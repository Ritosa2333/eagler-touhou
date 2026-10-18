"""Report real unresolved game dependencies from the compiled static library."""
from pathlib import Path
import argparse
import hashlib
import json
import re
import subprocess

HERE = Path(__file__).resolve().parent

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("library", type=Path)
    parser.add_argument("--dumpbin", required=True, type=Path)
    args = parser.parse_args()
    output = subprocess.check_output([str(args.dumpbin), "/symbols", str(args.library)], text=True, encoding="utf-8", errors="replace")
    undefined = {}
    defined = set()
    for line in output.splitlines():
        if "External" not in line or "|" not in line:
            continue
        symbol_text = line.split("|", 1)[1].strip()
        symbol = symbol_text.split()[0]
        if "UNDEF" in line:
            undefined[symbol] = symbol_text
        else:
            defined.add(symbol)
    unresolved = {symbol: text for symbol, text in undefined.items() if symbol not in defined}
    missing_functions = sorted(text for symbol, text in unresolved.items() if "@unrecovered@program_entry@source@th20@@" in symbol)
    missing_globals = sorted(text for symbol, text in unresolved.items() if "@program_entry@source@th20@@" in symbol and "@unrecovered@" not in symbol)
    other_modules = sorted(text for symbol, text in unresolved.items() if "@scheduler@source@th20@@" in symbol)
    platform_symbols = sorted(text for symbol, text in unresolved.items() if "@source@th20@@" not in symbol)
    record = {
        "schema": "th20.program_entry.source_module.v1",
        "status": "compiled_static_module_with_unrecovered_dependencies",
        "validation": "MSVC Win32 compilation and symbol audit only; no original-CPU/gameplay/COM equivalence test",
        "library_sha256": hashlib.sha256(args.library.read_bytes()).hexdigest(),
        "source_hashes": {p.name: hashlib.sha256(p.read_bytes()).hexdigest() for p in HERE.iterdir() if p.suffix in [".cpp", ".hpp"]},
        "unrecovered_function_count": len(missing_functions),
        "unrecovered_functions": missing_functions,
        "unresolved_global_count": len(missing_globals),
        "unresolved_globals": missing_globals,
        "implemented_in_core_scheduler": other_modules,
        "platform_or_compiler_symbols": platform_symbols,
        "independent_executable_linkable": False,
        "original_executable_needed_by_library": False,
    }
    (HERE / "module_status.json").write_text(json.dumps(record, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    print(f"Unrecovered functions: {len(missing_functions)}; undefined global storage/environment: {len(missing_globals)}; existing scheduler imports: {len(other_modules)}")

if __name__ == "__main__":
    main()
