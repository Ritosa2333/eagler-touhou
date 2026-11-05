"""Build a source candidate without touching deployed assets or global tools."""
from pathlib import Path
import os
import subprocess
import sys

ROOT = Path(__file__).resolve().parent.parent
WORK = ROOT / 'build/native-scheduler'
SDK = WORK / 'toolchain/emsdk-4.0.15'
CMAKE = WORK / 'python-tools/cmake/data/bin'
NINJA = WORK / 'python-tools/bin'
PYTHON = SDK / 'python/3.13.3_64bit/python.exe'
EM = SDK / 'upstream/emscripten'
env = dict(os.environ)
env.update(EMSDK=str(SDK), EM_CONFIG=str(SDK / '.emscripten'),
           EMSDK_NODE=str(SDK / 'node/22.16.0_64bit/bin/node.exe'),
           EMSDK_PYTHON=str(PYTHON))
env['EM_BINARYEN_ROOT'] = str(ROOT / 'build/binaryen-native-132/binaryen-version_132')
env['BINARYEN_CORES'] = '2'
raw = '--raw' in sys.argv
if raw:
    env['EM_BINARYEN_ROOT'] = str(SDK / 'upstream')
env['PATH'] = os.pathsep.join(map(str, [CMAKE, NINJA, EM, SDK])) + os.pathsep + env['PATH']
native = '--native' in sys.argv
output = WORK / ('out-native' if native else 'out-baseline')
subprocess.run([str(PYTHON), str(EM / 'emcmake.py'), str(CMAKE / 'cmake.exe'),
    '-S', str(WORK / 'source'), '-B', str(output), '-G', 'Ninja',
    '-DCMAKE_BUILD_TYPE=Release', '-DDOSBOX_DEBUG=OFF',
    '-DTH_NATIVE_SCHEDULER=' + ('ON' if native else 'OFF'),
    '-DCMAKE_MAKE_PROGRAM=' + str(NINJA / 'ninja.exe')], env=env, check=True)
subprocess.run([str(CMAKE / 'cmake.exe'), '--build', str(output),
    '--target', 'wdosbox-x-jspi' if raw else 'wdosbox-x', '-j', '1'], env=env, check=True)
