#!/usr/bin/env python3
"""Build a local SoEW standalone EXE from a statically linked app and staged assets."""
import argparse
import ctypes
from ctypes import wintypes
import hashlib
import os
from pathlib import Path
import re
import shutil
import stat
import struct
import subprocess
import tempfile

from stage_locale import CHINESE_MAX_SIZE, CHINESE_PACK, validate_chinese_overlay

HERE = Path(__file__).resolve().parent
ROOT = HERE.parent
SOUNDFONT = "GeneralUser-GS-SoftSynth-1.44.sf2"
SOUNDFONT_SHA = "00725296486f652ba42bdbf2098254762ca80239353d04b55a27454fc8edb8a8"
SLIM_SOUNDFONT_SHA = "3472ba3752c3d2c1f0afbbe1369b7ad1304161474b89cbb9f5bb50c5d492f0d4"
ASSET_TYPES = {".bbt", ".bft", ".bmt", ".btt", ".dt1", ".efc", ".m", ".map", ".mmd", ".mpn", ".pi", ".rec", ".rgb", ".txt"}
REQUIRED = {
    "HUUHI.DAT", "HUUMA.EFC", "MIKOFT.BFT", "MUSIC.TXT", "OP.PI", "OP2.PI", "OP3.PI", "OP.RGB",
    "OPA.PI", "OPB.PI", "OPC.PI", "OP_H.BFT", "OP_H.RGB", "TSELECT.PI", "TS1.PI", "TS2.PI", "TS3.PI",
    SOUNDFONT, *(f"STAGE{index}.{suffix}" for index in range(6) for suffix in ("BBT", "BFT", "BMT", "DT1", "M", "MAP", "MMD", "MPN", "TXT")),
}
LICENSES = {"98fmplayer": "98fmplayer/LICENSE", "TinySoundFont": "TinySoundFont/LICENSE", "ymfm": "ymfm/LICENSE"}
MODES = {"default": 0, "stage5-mainline": 1, "extra-candidate": 2, "portable-save": 3}
ALGORITHM = 5  # COMPRESS_ALGORITHM_LZMS
MMDC_V1_HEADER = struct.pack("<4sII", b"MMDC", 1, 48000)
MAX_FILE = 64 * 1024 * 1024
FILE_NAME = re.compile(r"[A-Za-z0-9_-]+(?:\.[A-Za-z0-9_-]+)+\Z", re.ASCII)
LOCALE_TEXT = {"MUSIC.TXT", *(f"STAGE{i}.TXT" for i in range(6)), *(f"END{i}.TXT" for i in range(1, 4))}
LOCALE_FILES = {"common": {"OP2.PI", "OP.RGB"}, "en": LOCALE_TEXT, "ja": LOCALE_TEXT}


def validate_checkpoint_soundfont(app, font):
    """Match the final font to the MMDC v1 checkpoint identities embedded in the native app."""
    known = {bytes.fromhex(SOUNDFONT_SHA), bytes.fromhex(SLIM_SOUNDFONT_SHA)}
    required = set()
    offset = app.find(MMDC_V1_HEADER)
    while offset != -1:
        # Magic, version, sample rate, song SHA-256, then SoundFont SHA-256.
        digest = app[offset + 44:offset + 76]
        if digest in known:
            required.add(digest)
        offset = app.find(MMDC_V1_HEADER, offset + len(MMDC_V1_HEADER))
    if required and required != {hashlib.sha256(font).digest()}:
        raise RuntimeError("SoundFont does not match the embedded private MIDI checkpoints; "
                           "pair --slim-midi with SOEW_PRIVATE_SLIM_MIDI_CHECKPOINTS")


def is_reparse(path):
    return bool(path.lstat().st_file_attributes & stat.FILE_ATTRIBUTE_REPARSE_POINT)


def toolchain(explicit_bin):
    """Use an explicit MinGW bin directory, a local checkout toolchain, or PATH."""
    if explicit_bin:
        tools = Path(os.path.abspath(explicit_bin))
    elif (ROOT / "tools/mingw64/bin/g++.exe").is_file():
        tools = ROOT / "tools/mingw64/bin"
    else:
        found = shutil.which("g++")
        if not found:
            raise RuntimeError("No MinGW-w64 toolchain found: pass --mingw-bin or put g++ on PATH")
        tools = Path(found).parent
    missing = [name for name in ("g++.exe", "windres.exe", "objdump.exe", "strip.exe") if not (tools / name).is_file()]
    if missing:
        raise RuntimeError(f"Incomplete MinGW-w64 toolchain at {tools}: {', '.join(missing)}")
    env = os.environ.copy()
    env["PATH"] = str(tools) + os.pathsep + env.get("PATH", "")
    gcc = tools.parent / "lib/gcc"
    if gcc.is_dir():
        env["GCC_EXEC_PREFIX"] = gcc.as_posix() + "/"
    return tools, env


def validate_imports(binary, tools, env):
    info = subprocess.run([str(tools / "objdump.exe"), "-p", str(binary)], env=env, check=True,
                          capture_output=True, text=True).stdout
    names = sorted(set(re.findall(r"DLL Name:\s*(\S+)", info, re.IGNORECASE)), key=str.casefold)
    system = Path(os.environ["SystemRoot"]) / "System32"
    if "file format pei-x86-64" not in info or not names:
        raise RuntimeError(f"Expected a Windows x64 executable with PE imports: {binary}")
    for name in names:
        if not re.fullmatch(r"[A-Za-z0-9_.-]+\.dll", name, re.ASCII | re.IGNORECASE):
            raise RuntimeError(f"Invalid PE import name: {name}")
        if not name.casefold().startswith(("api-ms-win-", "ext-ms-win-")) and not (system / name).is_file():
            raise RuntimeError(f"Non-system DLL imported by {binary.name}: {name}")
    print(f"{binary.name} imports: {', '.join(names)}")


def soundfont_notice(data):
    if hashlib.sha256(data).hexdigest() != SOUNDFONT_SHA:
        raise RuntimeError("Unexpected GeneralUser GS 1.44 SoundFont SHA-256")
    if data[:4] != b"RIFF" or data[8:12] != b"sfbk":
        raise RuntimeError("Invalid SoundFont RIFF header")

    def chunks(start, end):
        while start + 8 <= end:
            key, size = struct.unpack_from("<4sI", data, start)
            start += 8
            if size > end - start:
                raise RuntimeError("Truncated SoundFont RIFF chunk")
            yield key, start, size
            start += size + (size % 2)

    for key, offset, size in chunks(12, len(data)):
        if key == b"LIST" and size >= 4 and data[offset:offset + 4] == b"INFO":
            tags = {key: data[at:at + count].rstrip(b"\0") for key, at, count in chunks(offset + 4, offset + size)}
            if all(tags.get(tag) for tag in (b"INAM", b"IENG", b"ICMT", b"ICOP")):
                return b"\r\n\r\n".join(tag + b": " + tags[tag] for tag in (b"INAM", b"IENG", b"ICMT", b"ICOP")) + b"\r\n"
    raise RuntimeError("Embedded GeneralUser GS license missing from SoundFont")


def locale_payload(locale):
    def directory_files(directory, expected, optional=()):
        if is_reparse(directory) or not directory.is_dir():
            raise RuntimeError(f"Expected an unlinked locale directory: {directory}")
        entries = {entry.name: entry for entry in directory.iterdir()}
        unexpected = (entries.keys() - expected - set(optional)) | (expected - entries.keys())
        if unexpected:
            raise RuntimeError(f"Incomplete or unexpected locale assets in {directory}: "
                               f"{', '.join(sorted(unexpected))}")
        return entries

    def read_asset(path, cap=MAX_FILE):
        if is_reparse(path) or not path.is_file() or not 0 < path.stat().st_size <= cap:
            raise RuntimeError(f"Expected a regular locale asset of valid size: {path}")
        with path.open("rb") as stream:
            data = stream.read(cap + 1)
        if not 0 < len(data) <= cap:
            raise RuntimeError(f"Locale asset changed size: {path}")
        return data

    top = directory_files(locale, set(LOCALE_FILES) | {"manifest.json"}, optional=(CHINESE_PACK,))
    manifest = read_asset(top["manifest.json"], 256 * 1024)
    pack = read_asset(top[CHINESE_PACK], CHINESE_MAX_SIZE) if CHINESE_PACK in top else None
    validate_chinese_overlay(manifest, pack)
    files = [("content/locale/manifest.json", manifest)]
    if pack is not None:
        files.append((f"content/locale/{CHINESE_PACK}", pack))
    for folder, names in LOCALE_FILES.items():
        for name, file in directory_files(top[folder], names).items():
            files.append((f"content/locale/{folder}/{name}", read_asset(file)))
    return files


def payload_files(content, app, th98port, slim_midi=False):
    names = set()
    app_data = app.read_bytes()
    files = [("app/SoEW-Native.exe", app_data)]
    for path in sorted(content.iterdir(), key=lambda path: path.name.casefold()):
        if path.name == "locale":
            files.extend(locale_payload(path))
            continue
        if path.is_dir() or is_reparse(path):
            raise RuntimeError(f"Expected a flat, unlinked asset directory outside locale: {path}")
        name = path.name
        if name.casefold() == CHINESE_PACK:
            raise RuntimeError(f"Chinese sidecar belongs only in content/locale: {path}")
        if path.suffix.casefold() not in ASSET_TYPES and name.casefold() not in {"huuhi.dat", SOUNDFONT.casefold()}:
            continue  # Exclude provenance JSON, saves, logs and executable sidecars.
        if not FILE_NAME.fullmatch(name) or len(name) > 100:
            raise RuntimeError(f"Unsupported asset name: {name}")
        stem = name.split(".")[0].upper()
        if stem in {"AUX", "CON", "NUL", "PRN"} or re.fullmatch(r"(?:COM|LPT)[0-9]", stem):
            raise RuntimeError(f"Reserved Windows asset name: {name}")
        if name.casefold() in names:
            raise RuntimeError(f"Case-insensitive duplicate asset name: {name}")
        names.add(name.casefold())
        if not 0 < path.stat().st_size <= MAX_FILE:
            raise RuntimeError(f"Invalid asset size: {path}")
        data = path.read_bytes()
        if name.casefold() == SOUNDFONT.casefold():
            files.append(("licenses/GeneralUser-GS-1.44.txt", soundfont_notice(data)))
            if slim_midi:
                from slim_soundfont import slim_soundfont
                data, stats = slim_soundfont(data, content)
                print(f"TH02 MIDI subset: {stats['retained_pcm_bytes']:,} bytes of used PCM retained")
            validate_checkpoint_soundfont(app_data, data)
        files.append(("content/" + name, data))
    missing = sorted(name for name in REQUIRED if name.casefold() not in names)
    if missing:
        raise RuntimeError(f"Missing staged assets: {', '.join(missing)}")
    for label, path in LICENSES.items():
        files.append((f"licenses/{label}.txt", (th98port / "third_party" / path).read_bytes()))
    return sorted(files)


class Compressor:
    def __enter__(self):
        self.api = ctypes.WinDLL("cabinet", use_last_error=True)
        self.api.CreateCompressor.argtypes = [wintypes.DWORD, ctypes.c_void_p, ctypes.POINTER(wintypes.HANDLE)]
        self.api.CreateCompressor.restype = wintypes.BOOL
        self.api.Compress.argtypes = [wintypes.HANDLE, ctypes.c_void_p, ctypes.c_size_t,
                                      ctypes.c_void_p, ctypes.c_size_t, ctypes.POINTER(ctypes.c_size_t)]
        self.api.Compress.restype = wintypes.BOOL
        self.api.CloseCompressor.argtypes = [wintypes.HANDLE]
        self.api.CloseCompressor.restype = wintypes.BOOL
        self.handle = wintypes.HANDLE()
        if not self.api.CreateCompressor(ALGORITHM, None, ctypes.byref(self.handle)):
            raise ctypes.WinError(ctypes.get_last_error())
        return self

    def __exit__(self, *_):
        self.api.CloseCompressor(self.handle)

    def compress(self, data):
        source = ctypes.c_char_p(data)
        needed = ctypes.c_size_t()
        if not self.api.Compress(self.handle, source, len(data), None, 0, ctypes.byref(needed)):
            error = ctypes.get_last_error()
            if error != 122:
                raise ctypes.WinError(error)
        target = ctypes.create_string_buffer(needed.value)
        if not self.api.Compress(self.handle, source, len(data), target, len(target), ctypes.byref(needed)):
            raise ctypes.WinError(ctypes.get_last_error())
        return target.raw[:needed.value] if needed.value < len(data) else data


def write_overlay(executable, files, mode):
    # Manifest is uncompressed so warm startup can verify the cache without unpacking every entry.
    manifest = bytearray(struct.pack("<8sII", b"SOEWMF01", mode, len(files)))
    packed_hash = hashlib.sha256()
    with executable.open("ab") as target, Compressor() as compressor:
        start = target.tell()
        for name, data in files:
            if not 0 < len(data) <= MAX_FILE:
                raise RuntimeError(f"Invalid payload file size: {name}")
            encoded = name.encode("ascii")
            packed = compressor.compress(data)
            manifest.extend(struct.pack("<HII32s", len(encoded), len(data), len(packed), hashlib.sha256(data).digest()))
            manifest.extend(encoded)
            target.write(packed)
            packed_hash.update(packed)
        data_size = target.tell() - start
        packed_hash.update(manifest)
        digest = packed_hash.digest()
        target.write(manifest)
        target.write(struct.pack("<8sQII32s", b"SOEWEX01", start, len(manifest), ALGORITHM, digest))
    print(f"Packed {len(files)} files: {sum(len(data) for _, data in files):,} -> {data_size:,} bytes; package={digest.hex()}")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--native-app", type=Path, required=True, help="statically linked x64 SoEW native EXE")
    parser.add_argument("--content", type=Path, required=True, help="prepared flat SoEW runtime asset directory")
    parser.add_argument("--output", type=Path, required=True, help="standalone .exe; parent must exist")
    parser.add_argument("--icon", type=Path, default=ROOT / "src" / "app_icon.ico",
                        help="local .ico matching native CMake SOEW_ICON_FILE; defaults to the neutral icon")
    parser.add_argument("--th98port", type=Path, default=ROOT / "external/th98port",
                        help="th98port checkout used for third-party license files")
    parser.add_argument("--mingw-bin", type=Path, help="explicit MinGW-w64 bin directory (otherwise uses PATH or local tools)")
    parser.add_argument("--launch-mode", choices=MODES, default="extra-candidate", help="default interactive launch mode")
    parser.add_argument("--slim-midi", action="store_true", help="keep only the PCM and patches used by the validated TH02 soundtrack")
    parser.add_argument("--force", action="store_true", help="replace an existing standalone output")
    args = parser.parse_args()
    if os.name != "nt":
        raise RuntimeError("Packaging requires Windows x64 and its Compression API")
    app, content, output = (Path(os.path.abspath(p)) for p in (args.native_app, args.content, args.output))
    if not app.is_file() or is_reparse(app):
        raise RuntimeError(f"Native app must be a regular file: {app}")
    if not content.is_dir() or is_reparse(content):
        raise RuntimeError(f"Content must be a regular directory: {content}")
    if output.suffix.casefold() != ".exe" or not output.parent.is_dir():
        raise RuntimeError(f"Output must be an .exe in an existing directory: {output}")
    if output.resolve() == app.resolve() or output.resolve().is_relative_to(content.resolve()):
        raise RuntimeError("Output cannot replace the input app or live inside the staged content")
    if output.exists() and (not args.force or is_reparse(output)):
        raise RuntimeError(f"Output already exists: {output}")
    icon = Path(os.path.abspath(args.icon))
    th98port = Path(os.path.abspath(args.th98port))
    if icon.suffix.casefold() != ".ico" or not icon.is_file() or is_reparse(icon):
        raise RuntimeError(f"Icon must be a regular .ico file: {icon}")
    tools, env = toolchain(args.mingw_bin)
    validate_imports(app, tools, env)
    with tempfile.TemporaryDirectory(prefix=".soew-package-", dir=output.parent) as directory:
        work = Path(directory)
        copied_app = work / "native.exe"
        shutil.copyfile(app, copied_app)
        subprocess.run([str(tools / "strip.exe"), "--strip-all", str(copied_app)], env=env, check=True)
        files = payload_files(content, copied_app, th98port, args.slim_midi)
        launcher = work / "standalone.exe"
        for name in ("app_icon.rc", "app_icon_resource.h"):
            shutil.copyfile(ROOT / "src" / name, work / name)
        shutil.copyfile(icon, work / "app_icon.ico")
        icon_resource = work / "app_icon.o"
        subprocess.run([str(tools / "windres.exe"), "--input=app_icon.rc", "--output-format=coff",
                        "--output", icon_resource.name], cwd=work, env=env, check=True)
        link_flags = ["-static", "-mwindows", "-municode", "-Wl,--no-insert-timestamp"]
        manifest = tools.parent / "x86_64-w64-mingw32/lib/default-manifest.o"
        if any(character.isspace() for character in str(tools)) and manifest.is_file():
            # MinGW's default-manifest spec splits absolute paths at spaces.
            shutil.copyfile(manifest, work / manifest.name)
            link_flags.append("-B./")
        subprocess.run([str(tools / "g++.exe"), "-std=c++17", "-Wall", "-Wextra", "-Werror", "-Os", "-s",
                        *link_flags, str(HERE / "standalone_launcher.cpp"), icon_resource.name, "-o", launcher.name,
                        "-lcabinet", "-lbcrypt", "-lshell32", "-lole32", "-luuid"], cwd=work, env=env, check=True)
        validate_imports(launcher, tools, env)
        write_overlay(launcher, files, MODES[args.launch_mode])
        (os.replace if args.force else os.rename)(launcher, output)
    print(f"{output} ({output.stat().st_size:,} bytes; SHA-256 {hashlib.sha256(output.read_bytes()).hexdigest()})")


if __name__ == "__main__":
    main()
