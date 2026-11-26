"""网页发布的隐私检查与 ZIP 时间、属性规则。"""

from pathlib import Path

import getpass
import os
import re
import socket
import struct
import time
import zlib

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

def privacy_report(count):
    return {'result': 'passed', 'files_checked': count, 'file_contents': 'preserved byte for byte',
            'media_metadata': 'preserved', 'modification_times': 'preserved at ZIP two-second precision',
            'archive_attributes': 'relative paths; no source owner, ACL or alternate streams',
            'scan': 'local account/computer names, user-home paths, private-key/token signatures; UTF-8, UTF-16 and compressed PNG text',
            'excluded': 'user data, replays, logs, credential files, build intermediates, debug symbols and Git metadata'}

def zip_date(timestamp):
    date = time.localtime(timestamp)[:6]
    if date[0] < 1980:
        return (1980, 1, 1, 0, 0, 0)
    if date[0] > 2107:
        return (2107, 12, 31, 23, 59, 58)
    return date
