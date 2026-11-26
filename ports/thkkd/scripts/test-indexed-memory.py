"""在固定 WASM 工具链上执行原映射与 Web 索引映射的差分用例。"""
from pathlib import Path
import os,subprocess
port=Path(__file__).resolve().parents[1];repo=port.parents[1];sdk=Path(os.environ.get('EMSDK',repo/'toolchains/emsdk'))
version=(sdk/'upstream/emscripten/emscripten-version.txt').read_text().strip().strip('"')
if version!='6.0.9':raise RuntimeError('Expected Emscripten 6.0.9, got '+version)
out=port/'build/memory-test';out.mkdir(parents=True,exist_ok=True)
temp=out/'tmp';temp.mkdir(exist_ok=True)
env={**os.environ,'EM_CONFIG':str(sdk/'.emscripten'),'TEMP':str(temp),'TMP':str(temp)}
subprocess.run(['python',str(sdk/'upstream/emscripten/em++.py'),'-std=c++17','-O2','-g0','-fexceptions','-fno-fast-math','-ffp-contract=off','-Wno-c++11-narrowing','-I'+str(port/'src'),str(port/'tests/support/indexed_memory.cpp'),'-sENVIRONMENT=node','-sEXIT_RUNTIME=1','-o',str(out/'test.cjs')],cwd=port,env=env,check=True)
subprocess.run(['node',str(out/'test.cjs')],check=True)
