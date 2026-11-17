"""Select the original art table while the core redraws the local player."""
import argparse
import re
from pathlib import Path

parser = argparse.ArgumentParser()
parser.add_argument('--input', type=Path, required=True)
parser.add_argument('--output', type=Path, required=True)
parser.add_argument('--function', required=True)
parser.add_argument('--tracked', action='store_true')
parser.add_argument('--control-local', type=int, default=None)
args = parser.parse_args()

source = args.input.read_text(encoding='utf-8')
start = source.index(' (func $' + args.function + ' ')
end = source.find('\n (func ', start + 1)
if end < 0:
    end = source.index('\n ;; custom section', start)
function = source[start:end]
locals_found = [int(value) for value in re.findall(r'\(local \$(\d+) i32\)', function)]
saved = max(locals_found) + 1
last_local = f'  (local ${max(locals_found)} i32)\n'
function = function.replace(last_local, last_local + f'  (local ${saved} i32)\n', 1)

if args.tracked:
    loop = '   (loop $label1\n'
    control = args.control_local if args.control_local is not None else 9
    before = (f'   (local.set ${saved} (i32.load $0 offset=4 (local.get ${control})))\n'
              f'   (i32.store $0 offset=4 (local.get ${control}) (i32.load $0 offset=28 (local.get ${control})))\n')
    restore = f'  (if (local.get ${saved}) (then (i32.store $0 offset=4 (local.get ${control}) (local.get ${saved}))))\n'
else:
    loop = '  (loop $label\n'
    before = (f'  (local.set ${saved} (i32.load offset=4 (local.get $2)))\n'
              '  (i32.store offset=4 (local.get $2) (i32.load offset=28 (local.get $2)))\n')
    restore = f'  (i32.store offset=4 (local.get $2) (local.get ${saved}))\n'
if function.count(loop) != 1:
    raise ValueError('local layer loop does not match the expected core')
function = function.replace(loop, before + loop, 1)
closing = function.rfind('\n )')
function = function[:closing] + '\n' + restore + function[closing:]
args.output.write_text(source[:start] + function + source[end:], encoding='utf-8', newline='\n')
