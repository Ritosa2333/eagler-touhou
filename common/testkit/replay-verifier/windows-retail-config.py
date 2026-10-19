"""Fail before launch if a retail game could fall back to fullscreen."""
import ctypes
import struct
from pathlib import Path


def config_name(game, code_page):
    if game == 7:
        return 'th07.cfg'
    if game != 6:
        raise ValueError('Unsupported retail game')
    # The Japanese EXE passes these Shift-JIS bytes to an ANSI file API.
    # This must match the process ACP, not the spelling of a guessed Unicode file.
    codec = 'utf-8' if code_page == 65001 else f'cp{code_page}'
    return '東方紅魔郷.cfg'.encode('cp932').decode(codec, errors='replace')


def validate_config(game, data):
    if game not in (6, 7):
        raise ValueError('Unsupported retail game')
    version = 0x102 if game == 6 else 0x70002
    start = 24 if game == 6 else 28
    if len(data) != 56 or struct.unpack_from('<I', data, 20)[0] != version:
        raise ValueError('Missing/corrupt retail config would reset to fullscreen')
    bounds = [5, 4, 2, 3, 2, 5 if game == 6 else 6, 2, 3]
    if game == 7:
        bounds += [3, 2, 2]
    if any(data[start + i] >= bound for i, bound in enumerate(bounds)):
        raise ValueError('Invalid retail option would reset the config to fullscreen')
    if data[start + 6] != 1:
        raise ValueError('Retail config must explicitly select windowed mode')


def require_windowed(game, directory):
    if not hasattr(ctypes, 'windll'):
        raise RuntimeError('Retail launch requires Windows')
    code_page = ctypes.windll.kernel32.GetACP()
    path = Path(directory) / config_name(game, code_page)
    if not path.is_file():
        raise ValueError(f'Retail windowed config not found for ACP {code_page}: {path}')
    validate_config(game, path.read_bytes())
    return path
