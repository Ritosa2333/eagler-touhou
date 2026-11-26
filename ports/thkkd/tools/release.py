"""检查发布隐私，保留资源原有信息和修改时间，生成源码及 Windows 游戏包。"""
from pathlib import Path
import argparse
import datetime
import getpass
import hashlib
import json
import os
import re
import socket
import struct
import time
import zipfile
import zlib

PROJECT = Path(__file__).resolve().parents[1]
SKIP_DIRS = {'build', 'dist', 'userdata', 'replay', '.git', '.github-private', '__pycache__', '.vs', '.idea'}
SKIP_SUFFIXES = {'.pdb', '.ilk', '.obj', '.log', '.pyc', '.user', '.suo', '.rpy'}
SKIP_NAMES = {'AGENTS.md', 'PACKAGE-MANIFEST.json', 'PRIVACY-CHECK.json', '.env', '.git-credentials',
              'credentials.json', 'credentials.ini', 'id_rsa', 'id_dsa', 'id_ecdsa', 'id_ed25519'}
PERSONAL_PATHS = re.compile(rb'(?:[a-z]:[\\/](?:users|documents and settings)[\\/][^\\/\x00\r\n ]+|/(?:users|home)/[a-z0-9_.-]+)')
SECRETS = re.compile(rb'(?:-----BEGIN (?:RSA |EC |OPENSSH |DSA )?PRIVATE KEY-----|(?<![A-Za-z0-9_])gh[pousr]_[A-Za-z0-9]{30,}|(?<![A-Za-z0-9_])github_pat_[A-Za-z0-9_]{20,}|(?<![A-Za-z0-9])(?:AKIA|ASIA)[0-9A-Z]{16}(?![A-Za-z0-9]))')

def local_identity_needles():
    values = {os.environ.get('USERNAME', ''), os.environ.get('COMPUTERNAME', ''),
              getpass.getuser(), Path.home().name, socket.gethostname()}
    return tuple({value.casefold().encode(encoding)
                  for value in values if len(value) >= 4
                  for encoding in ('utf-8', 'utf-16le', 'utf-16be')})

IDENTITY_NEEDLES = local_identity_needles()

def files_in(root):
    for folder, dirs, names in os.walk(root):
        dirs[:] = sorted(d for d in dirs if d not in SKIP_DIRS and not (Path(folder).resolve() == PROJECT and d in {'assets', 'content'}))
        for name in sorted(names):
            path = Path(folder)/name
            private_env = name.startswith('.env.') and not name.endswith(('.example', '.template'))
            if path.suffix.lower() not in SKIP_SUFFIXES and name not in SKIP_NAMES and not private_env and not name.endswith('.release-tmp'):
                yield path

def check_bytes(data, relative_path):
    lowered = data.lower()
    if any(needle in lowered for needle in IDENTITY_NEEDLES):
        raise ValueError(f'Local account or computer name in {relative_path}')
    for raw in (data, data[::2], data[1::2]):
        view = raw.lower()
        if any(prefix in view for prefix in (b'users', b'documents and settings', b'/home/')) and PERSONAL_PATHS.search(view):
            raise ValueError(f'Personal directory in {relative_path}')
        if any(prefix in view for prefix in (b'private key', b'ghp_', b'gho_', b'ghu_', b'ghs_', b'ghr_', b'github_pat_', b'akia', b'asia')) and SECRETS.search(raw):
            raise ValueError(f'Private key or access token in {relative_path}')

def png_compressed_text(data):
    if not data.startswith(b'\x89PNG\r\n\x1a\n'):
        return
    offset = 8
    while offset + 12 <= len(data):
        size = struct.unpack_from('>I', data, offset)[0]
        kind = data[offset+4:offset+8]
        chunk = data[offset+8:offset+8+size]
        if len(chunk) != size:
            raise ValueError('Incomplete PNG metadata')
        if kind == b'zTXt':
            _, payload = chunk.split(b'\0', 1)
            if payload[0] != 0:
                raise ValueError('Unsupported PNG text compression')
            yield zlib.decompress(payload[1:])
        elif kind == b'iTXt':
            _, payload = chunk.split(b'\0', 1)
            compressed, method = payload[:2]
            _, _, text = payload[2:].split(b'\0', 2)
            if compressed:
                if method != 0:
                    raise ValueError('Unsupported PNG text compression')
                yield zlib.decompress(text)
        offset += size + 12
        if kind == b'IEND':
            break

def checked_contents(path, root):
    relative = path.relative_to(root).as_posix()
    check_bytes(relative.encode('utf-8'), relative)
    data = path.read_bytes()
    check_bytes(data, relative)
    for text in png_compressed_text(data):
        check_bytes(text, relative)
    return data

def privacy_report(count):
    return {'result': 'passed', 'files_checked': count, 'file_contents': 'preserved byte for byte',
            'media_metadata': 'preserved', 'modification_times': 'preserved at ZIP two-second precision',
            'archive_attributes': 'relative paths; no source owner, ACL or alternate streams',
            'scan': 'local account/computer names, user-home paths, private-key/token signatures; UTF-8, UTF-16 and compressed PNG text',
            'excluded': 'user data, replays, logs, credential files, build intermediates, debug symbols and Git metadata'}

def check_directory(root):
    count = 0
    for path in files_in(root):
        checked_contents(path, root)
        count += 1
    return privacy_report(count)

def verify_readme(path):
    if not path.read_text(encoding='utf-8-sig').strip():
        raise ValueError(f'README is empty in {path.name}')

def copy_release_file(source, destination):
    timestamp = source.stat()
    destination.parent.mkdir(parents=True, exist_ok=True)
    temporary = destination.with_name(destination.name+'.release-tmp')
    temporary.write_bytes(source.read_bytes())
    os.utime(temporary, ns=(timestamp.st_atime_ns, timestamp.st_mtime_ns))
    for attempt in range(6):
        try:
            temporary.replace(destination)
            return
        except PermissionError:
            if attempt == 5:
                raise
            time.sleep(0.15 * (attempt + 1))

def prepare_game(root):
    root = root.resolve()
    if root == PROJECT or root.is_relative_to(PROJECT/'assets') or PROJECT.is_relative_to(root):
        raise ValueError('Choose a separate game output directory')
    pairs = [(PROJECT/'build/windows/bin/Release'/name, Path(name)) for name in ('thkkd.exe', 'thkkd-config.exe')]
    pairs += [(PROJECT/'docs/PLAY.txt', Path('README.txt')), (PROJECT/'THIRD_PARTY.md', Path('THIRD_PARTY.md'))]
    pairs += [(PROJECT/'third_party/dxlib'/name, Path('third_party/dxlib')/name) for name in ('DxLib.txt', 'VERSION.json')]
    pairs += [(path, path.relative_to(PROJECT)) for path in files_in(PROJECT/'assets')]
    for source, _ in pairs:
        if not source.is_file():
            raise FileNotFoundError(f'Build the game first; missing {source.relative_to(PROJECT)}')
    for source, relative in pairs:
        copy_release_file(source, root/relative)
    return len(pairs)

def zip_date(timestamp):
    date = time.localtime(timestamp)[:6]
    if date[0] < 1980:
        return (1980, 1, 1, 0, 0, 0)
    if date[0] > 2107:
        return (2107, 12, 31, 23, 59, 58)
    return date

def package(source, target, prefix):
    if target.exists():
        raise FileExistsError(f'Package already exists: {target.name}')
    temporary = target.with_name(target.name+'.partial')
    manifest = []
    try:
        with zipfile.ZipFile(temporary, 'x', compression=zipfile.ZIP_DEFLATED, compresslevel=1, allowZip64=True) as output:
            def write(name, data, modified, attributes=0x20):
                entry = zipfile.ZipInfo(prefix+'/'+name, zip_date(modified))
                entry.create_system = 0
                entry.external_attr = attributes or 0x20
                entry.compress_type = zipfile.ZIP_STORED if Path(name).suffix.lower() in {'.png', '.jpg', '.mp3', '.wmv', '.zip'} else zipfile.ZIP_DEFLATED
                output.writestr(entry, data, compresslevel=1)
            for path in files_in(source):
                name = path.relative_to(source).as_posix()
                data = checked_contents(path, source)
                stat = path.stat()
                write(name, data, stat.st_mtime, getattr(stat, 'st_file_attributes', 0x20) & 0x27)
                manifest.append({'path': prefix+'/'+name, 'bytes': len(data), 'sha256': hashlib.sha256(data).hexdigest(), 'modified_time_ns': stat.st_mtime_ns})
            report = privacy_report(len(manifest))
            now = time.time()
            write('PACKAGE-MANIFEST.json', json.dumps({'files': manifest}, ensure_ascii=False, indent=2).encode('utf-8'), now)
            write('PRIVACY-CHECK.json', json.dumps(report, ensure_ascii=False, indent=2).encode('utf-8'), now)
        temporary.replace(target)
    except BaseException:
        if temporary.exists():
            temporary.unlink()
        raise
    with target.open('rb') as stream:
        checksum = hashlib.file_digest(stream, 'sha256').hexdigest()
    target.with_suffix(target.suffix+'.sha256').write_text(checksum+'  '+target.name+'\n', encoding='utf-8')
    return {'path': target.name, 'files': len(manifest)+2, 'bytes': target.stat().st_size, 'sha256': checksum, 'privacy': report}

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--game-dir', type=Path, default=PROJECT/'dist/thkkd-windows-x64')
    parser.add_argument('--output', type=Path, default=PROJECT/'dist/packages')
    parser.add_argument('--label', default=datetime.date.today().strftime('%Y%m%d'))
    mode = parser.add_mutually_exclusive_group()
    mode.add_argument('--check-only', action='store_true')
    mode.add_argument('--prepare-only', action='store_true')
    mode.add_argument('--check-source', action='store_true')
    args = parser.parse_args()
    verify_readme(PROJECT/'README.md')
    if args.check_source:
        print(json.dumps(check_directory(PROJECT), ensure_ascii=False, indent=2))
        return
    if args.prepare_only:
        prepare_game(args.game_dir)
    verify_readme(args.game_dir/'README.txt')
    if args.check_only or args.prepare_only:
        print(json.dumps(check_directory(args.game_dir), ensure_ascii=False, indent=2))
        return
    args.output.mkdir(parents=True, exist_ok=True)
    packages = [package(PROJECT, args.output/f'thkkd-源码-{args.label}.zip', 'thkkd'),
                package(args.game_dir, args.output/f'thkkd-Windows游戏包-{args.label}.zip', 'thkkd-windows-x64')]
    report = {'label': args.label, 'policy': 'preserve media information and modification times; check privacy', 'packages': packages}
    (args.output/'thkkd-release.json').write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')
    print(json.dumps([{k: p[k] for k in ('path', 'files', 'bytes', 'sha256')} for p in packages], ensure_ascii=False, indent=2))

if __name__ == '__main__':
    main()
