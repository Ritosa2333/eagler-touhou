"""Assemble the pinned core and an isolated, Windows-buildable CMake project.

Never edits vendor-source or the deployed web runtime. Re-running is safe.
"""
from pathlib import Path
import shutil
import zipfile

ROOT = Path(__file__).resolve().parent.parent
WORK = ROOT / 'build/native-scheduler'
SOURCE = WORK / 'source'
EMULATORS = ROOT / 'vendor-source/core-source/emulators-6bbf0925d221a7304ea8e0dda1e89b8dc01569b2'
DOSBOX = ROOT / 'vendor-source/core-source/dosbox-x-788fde99d3d451eab3621009186e5abe516f2b9c'

if not SOURCE.exists():
    shutil.copytree(EMULATORS, SOURCE)
if not (SOURCE / 'native/dosbox-x/src/dosbox.cpp').exists():
    shutil.copytree(DOSBOX, SOURCE / 'native/dosbox-x', dirs_exist_ok=True)
GL = WORK / 'gl4es-a744af14d4afbda77bf472bc53f43b9ceba39cc0'
if not GL.exists():
    with zipfile.ZipFile(WORK / 'gl4es-a744af1.zip') as archive:
        archive.extractall(WORK)

cmake = (EMULATORS / 'CMakeLists.txt').read_text()
start = cmake.index('\tExternalProject_Add(gl4es')
end = cmake.index('\nendif()', start)
cmake = cmake[:start] + '''\tExternalProject_Add(gl4es
        SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/../gl4es-a744af14d4afbda77bf472bc53f43b9ceba39cc0"
        BINARY_DIR "${GL4ES_INSTALL_DIR}"
        DOWNLOAD_COMMAND ""
        UPDATE_COMMAND ""
        CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
            -DCMAKE_BUILD_TYPE=Release -DNOX11=ON -DNOEGL=ON -DSTATICLIB=ON
            -DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}
        BUILD_BYPRODUCTS "${GL4ES_LIBRARY}"
        INSTALL_COMMAND "")
    set(GL4ES_INCLUDE "${CMAKE_CURRENT_LIST_DIR}/../gl4es-a744af14d4afbda77bf472bc53f43b9ceba39cc0/include")
''' + cmake[end:]
for target in ['dosbox', 'dhry2', 'sokol', 'dosbox-sdl']:
    cmake = cmake.replace(f'include("${{TARGETS_DIR}}/{target}.cmake")', '')
cmake = cmake[:cmake.index('add_custom_target(sokol-shaders')]
cmake = cmake.replace('set(GL4ES_LIBRARY "${GL4ES_INSTALL_DIR}/lib/libGL.a")',
    'set(GL4ES_LIBRARY "${CMAKE_CURRENT_LIST_DIR}/../gl4es-a744af14d4afbda77bf472bc53f43b9ceba39cc0/lib/libGL.a")')
cmake += '\noption(TH_NATIVE_SCHEDULER "Native deterministic clock and deadline" OFF)\n'
cmake += 'if(TH_NATIVE_SCHEDULER)\n add_compile_definitions(TH_NATIVE_SCHEDULER=1)\nendif()\n'
(SOURCE / 'CMakeLists.txt').write_text(cmake)
# The shared event file only needs DOSBox-X's matching CPU declarations.
events = SOURCE / 'native/jsdos/jsdos-events.cpp'
events.write_text(events.read_text().replace('../dosbox/include/cpu.h', '../dosbox-x/include/cpu.h'))
print(SOURCE)
