"""核对内置回退字体对核心文本和显示常量字符串的字符覆盖。"""
import argparse
import hashlib
import json
from pathlib import Path
import re
from fontTools.ttLib import TTFont

ROOT = Path(__file__).resolve().parents[1]


def check_font():
    font_path = ROOT / 'third_party/unifont.woff2'
    with TTFont(font_path) as font:
        coverage = set(font.getBestCmap())
    strings = []
    for path in (ROOT / 'src').rglob('*.hpp'):
        for value in re.findall(r'return\s*\{\s*"((?:\\x[0-9a-fA-F]{2})+)"\s*,\s*\d+\s*\}', path.read_text(encoding='utf-8')):
            strings.append(bytes.fromhex(value.replace('\\x', '')).decode('gbk'))
    core_strings = len(strings)
    raw = bytes(int(value, 16) for value in re.findall(r'0x([0-9a-f]{2})', (ROOT / 'src/render/DisplayConstants.inc').read_text()))
    base = 0x6a0000
    addresses = set()
    for path in (ROOT / 'src/render').glob('*.cpp'):
        addresses.update(int(value, 16) for value in re.findall(r'0x([0-9a-f]{6})u', path.read_text()) if base <= int(value, 16) < base + len(raw))
    for address in sorted(addresses):
        data = raw[address - base:address - base + 2048].split(b'\0', 1)[0]
        try:
            text = data.decode('gbk')
        except UnicodeDecodeError:
            continue
        if len(text) > 1 and all(char.isprintable() or char in '\n\r\t' for char in text):
            strings.append(text)
    display_strings = len(strings) - core_strings
    strings.append('自由移动 · 处理落率 100%')
    characters = {ord(char) for text in strings for char in text if char.isprintable() and not char.isspace()}
    missing = [f'U+{value:04X}' for value in sorted(characters - coverage)]
    return {'schema': 'thkkd/font-coverage/1', 'result': 'failed' if missing else 'passed',
            'coreStrings': core_strings, 'displayConstantStrings': display_strings,
            'uniqueCharacters': len(characters), 'missing': missing,
            'fontSha256': hashlib.sha256(font_path.read_bytes()).hexdigest()}


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output', type=Path)
    args = parser.parse_args()
    report = check_font()
    text = json.dumps(report, ensure_ascii=False, indent=2) + '\n'
    if args.output:
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_text(text, encoding='utf-8')
    print(text, end='')
    raise SystemExit(report['result'] != 'passed')
