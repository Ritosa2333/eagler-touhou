"""验证 Omniverse 收取结束后，普通游玩进入保存询问，录像返回主菜单。"""
from pathlib import Path
import os
import subprocess
import sys

port = Path(__file__).resolve().parents[1]
sdk = Path(os.environ.get('EMSDK', port.parents[1] / 'toolchains/emsdk'))
out = port / 'build/omniverse-clear-test'
out.mkdir(parents=True, exist_ok=True)
temporary = out / 'tmp'
temporary.mkdir(exist_ok=True)
env = {**os.environ, 'EM_CONFIG': str(sdk / '.emscripten'), 'TEMP': str(temporary), 'TMP': str(temporary)}
subprocess.run([sys.executable, str(sdk / 'upstream/emscripten/em++.py'), '-std=c++17', '-O2', '-g0',
                '-fexceptions', '-fno-fast-math', '-ffp-contract=off', '-Wno-c++11-narrowing',
                '-include', str(port / 'src/platform/web/WebMemory.hpp'), '-I' + str(port / 'src'),
                str(port / 'tests/support/omniverse_clear.cpp'), '-sENVIRONMENT=node', '-sEXIT_RUNTIME=1',
                '-sSTACK_SIZE=8388608', '-sALLOW_MEMORY_GROWTH=1', '-o', str(out / 'test.cjs')],
               cwd=port, env=env, check=True)
subprocess.run(['node', str(out / 'test.cjs')], check=True)
