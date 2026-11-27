"""使用隔离的 Windows/MSYS2 工具链重建海恵堂视频转换内核。"""
from pathlib import Path
import argparse
import ctypes
import gzip
import hashlib
import json
import os
import re
import shutil
import subprocess
import sys
import tarfile
import urllib.request
import zipfile

HERE = Path(__file__).resolve().parent
LOCK = json.loads((HERE / 'sources.lock.json').read_text(encoding='utf-8'))


def digest(path):
    with path.open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def run(command, **kwargs):
    result = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                            creationflags=subprocess.CREATE_NO_WINDOW, **kwargs)
    sys.stdout.buffer.write(result.stdout)
    sys.stdout.flush()
    result.check_returncode()
    return result


def download(item, cache):
    path = cache / item['archive']
    if path.exists():
        if digest(path) != item['sha256']:
            raise ValueError('缓存校验失败：' + path.name)
        return path
    temporary = path.with_name(path.name + '.part')
    error = None
    for url in [item['url'], *([item['mirror']] if 'mirror' in item else [])]:
        try:
            with urllib.request.urlopen(url, timeout=30) as response, temporary.open('wb') as stream:
                shutil.copyfileobj(response, stream)
            if digest(temporary) != item['sha256']:
                raise ValueError('下载校验失败：' + path.name)
            temporary.replace(path)
            return path
        except Exception as failure:
            error = failure
    raise error


def unzip(path, target, prefix=''):
    target.mkdir(parents=True, exist_ok=True)
    with zipfile.ZipFile(path) as archive:
        for item in archive.infolist():
            if item.is_dir() or not item.filename.startswith(prefix):
                continue
            name = item.filename[len(prefix):]
            dest = (target / name).resolve()
            if not dest.is_relative_to(target.resolve()):
                raise ValueError('ZIP 路径超出工具目录')
            dest.parent.mkdir(parents=True, exist_ok=True)
            with archive.open(item) as source, dest.open('wb') as sink:
                shutil.copyfileobj(source, sink)


def mount_drive(root, preferred):
    kernel = ctypes.windll.kernel32
    for drive in [preferred.upper(), *[c for c in 'RSTUVWXYZ' if c != preferred.upper()]]:
        buffer = ctypes.create_unicode_buffer(32768)
        occupied = kernel.QueryDosDeviceW(drive + ':', buffer, len(buffer))
        if occupied:
            existing = buffer.value.removeprefix('\\??\\')
            if Path(existing).resolve() == root:
                return drive, False
            continue
        run(['subst', drive + ':', str(root)])
        return drive, True
    raise RuntimeError('没有可用的临时构建盘符')


def prepare(root, drive):
    cache = root / 'downloads'
    cache.mkdir(parents=True, exist_ok=True)
    msys = root / 'msys64'
    if not (msys / 'usr/bin/bash.exe').exists():
        archive = download(LOCK['windows']['msys'], cache)
        with tarfile.open(archive) as source:
            if any(not (root / item.name).resolve().is_relative_to(root) for item in source.getmembers()):
                raise ValueError('MSYS2 路径超出工具目录')
        run(['tar', '-xf', str(archive), '-C', str(root)])
    sdk = root / 'emsdk'
    targets = [sdk / 'upstream', sdk / 'node/24.19.0_64bit', sdk / 'python/3.13.3_64bit']
    prefixes = ['install/', 'node-v24.19.0-win-x64/', '']
    checks = ['emscripten/emcc.py', 'node.exe', 'python.exe']
    for item, target, prefix, check in zip(LOCK['windows']['sdk'], targets, prefixes, checks):
        if not (target / check).exists():
            unzip(download(item, cache), target, prefix)
    version = (sdk / 'upstream/emscripten/emscripten-version.txt').read_text(encoding='utf-8').strip().strip('"')
    if version != LOCK['emscriptenVersion']:
        raise ValueError('Emscripten 版本与锁文件不一致')
    sdk.mkdir(exist_ok=True)
    (sdk / '.emscripten').write_text("import os\nroot = os.path.dirname(os.environ['EM_CONFIG'])\nNODE_JS = root + '/node/24.19.0_64bit/node.exe'\nPYTHON = root + '/python/3.13.3_64bit/python.exe'\nLLVM_ROOT = root + '/upstream/bin'\nBINARYEN_ROOT = root + '/upstream'\nEMSCRIPTEN_ROOT = root + '/upstream/emscripten'\n", encoding='utf-8', newline='\n')
    bash = f'{drive}:/msys64/usr/bin/bash.exe'
    packages = []
    for item in LOCK['windows']['packages']:
        package = download(item, cache)
        packages.append(f'/{drive.lower()}/downloads/{package.name}')
    # 首次执行便携工具后，再进行安装和配置；所有状态留在工具目录。
    warm = 'export PATH=/usr/bin:/bin; for tool in cat cp mv rm mkdir touch chmod ln install sed grep head tail sort cut; do /usr/bin/$tool --version >/dev/null; done'
    run([bash, '--noprofile', '--norc', '-c', warm])
    run([bash, '--noprofile', '--norc', '-c', 'export PATH=/usr/bin:/bin; pacman-key --init && pacman-key --populate msys2'])
    run([bash, '--noprofile', '--norc', '-c', 'export PATH=/usr/bin:/bin; pacman -U --noconfirm --needed ' + ' '.join(packages)])
    sources = root / 'sources'
    sources.mkdir(exist_ok=True)
    for item in LOCK['sources']:
        archive = download(item, cache)
        with tarfile.open(archive) as source:
            first = source.getnames()[0].split('/')[0]
            if not (sources / first).exists():
                source.extractall(sources, filter='data')
    build = msys / 'build'
    (build / 'src').mkdir(parents=True, exist_ok=True)
    for target, source in [('ffmpeg', 'ffmpeg-' + LOCK['ffmpegVersion']), ('libvpx', 'libvpx-' + LOCK['libvpxVersion'])]:
        if not (build / 'src' / target).exists():
            shutil.copytree(sources / source, build / 'src' / target)
    upstream = sources / ('ffmpeg.wasm-' + LOCK['ffmpegWasmRevision']) / 'src'
    ffmpeg = build / 'src/ffmpeg'
    for name in ['fftools', 'bind']:
        for source in (upstream / name).rglob('*'):
            if source.is_file():
                target = ffmpeg / 'src' / source.relative_to(upstream)
                target.parent.mkdir(parents=True, exist_ok=True)
                if not target.exists() or digest(target) != digest(source):
                    shutil.copy2(source, target)
    binding = ffmpeg / 'src/bind/ffmpeg/bind.js'
    text = binding.read_text(encoding='utf-8')
    start, end = text.index('function ffprobe('), text.index('function setLogger(')
    text = text[:start] + text[end:]
    for line in ['const DEFAULT_ARGS_FFPROBE = ["./ffprobe"];', 'Module["DEFAULT_ARGS_FFPROBE"] = DEFAULT_ARGS_FFPROBE;', 'Module["ffprobe"] = ffprobe;']:
        text = text.replace(line + '\n', '')
    binding.write_text(text, encoding='utf-8', newline='\n')
    shims = build / 'shims'
    shims.mkdir(exist_ok=True)
    for tool in ['emcc', 'em++', 'emar', 'emranlib', 'emnm']:
        script = 'tools/emnm.py' if tool == 'emnm' else tool + '.py'
        text = f'#!/usr/bin/bash\nexec /{drive.lower()}/emsdk/python/3.13.3_64bit/python.exe {drive}:/emsdk/upstream/emscripten/{script} "$@"\n'
        (shims / tool).write_text(text, encoding='utf-8', newline='\n')
    shutil.copy2(HERE / 'build.sh', build / 'build-core.sh')
    return build, bash


def record(build):
    component_source = (build / 'src/ffmpeg/config_components.h').read_text(encoding='utf-8')
    kinds = ['DECODER', 'ENCODER', 'DEMUXER', 'MUXER', 'PARSER', 'FILTER', 'PROTOCOL']
    components = {kind.lower(): re.findall(r'^#define CONFIG_(\w+_' + kind + r') 1$', component_source, re.M) for kind in kinds}
    files = {}
    for name in ['ffmpeg-core.js', 'ffmpeg-core.wasm']:
        source = build / 'output' / name
        files[name] = {'bytes': source.stat().st_size, 'sha256': digest(source), 'gzipBytes': len(gzip.compress(source.read_bytes(), compresslevel=9, mtime=0))}
    if files['ffmpeg-core.wasm']['bytes'] > 8 * 1024 * 1024 or files['ffmpeg-core.wasm']['gzipBytes'] > 3 * 1024 * 1024:
        raise ValueError('视频内核超过体积预算')
    vpx = (build / 'src/libvpx/vpx_config.h').read_text(encoding='utf-8')
    enabled = lambda name: bool(re.search(r'^#define CONFIG_' + name + r' 1$', vpx, re.M))
    license_line = next(line for line in (build / 'src/ffmpeg/config.h').read_text(encoding='utf-8').splitlines() if line.startswith('#define FFMPEG_LICENSE '))
    if license_line != '#define FFMPEG_LICENSE "LGPL version 2.1 or later"':
        raise ValueError('FFmpeg 构建许可与登记不一致')
    return {'schema': 'thkkd/video-core-build/1', 'license': 'LGPL-2.1-or-later', 'toolchain': 'Emscripten ' + LOCK['emscriptenVersion'],
            'upstream': {'ffmpeg': LOCK['ffmpegVersion'], 'libvpx': LOCK['libvpxVersion'], 'ffmpegWasmCore': LOCK['ffmpegWasmCoreVersion'], 'ffmpegWasmRevision': LOCK['ffmpegWasmRevision']},
            'singleThread': True, 'simd': True, 'components': components,
            'libvpx': {'vp8Encoder': enabled('VP8_ENCODER'), 'vp8Decoder': enabled('VP8_DECODER'), 'vp9Encoder': enabled('VP9_ENCODER'), 'vp9Decoder': enabled('VP9_DECODER'), 'multithread': enabled('MULTITHREAD')},
            'recipe': {name: digest(HERE / name) for name in ['sources.lock.json', 'build.sh', 'build.py']}, 'files': files}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--tools-dir', type=Path, required=True)
    parser.add_argument('--drive', default='R', choices=list('RSTUVWXYZ'))
    parser.add_argument('--verify', action='store_true', help='重建后比较已登记产物，保留已交付文件')
    options = parser.parse_args()
    if os.name != 'nt':
        parser.error('此入口使用 Windows 便携 MSYS2；编译步骤见 build.sh')
    root = options.tools_dir.resolve()
    root.mkdir(parents=True, exist_ok=True)
    drive, owned = mount_drive(root, options.drive)
    try:
        build, bash = prepare(root, drive)
        env = {**os.environ, 'THKKD_EMSDK_POSIX': f'/{drive.lower()}/emsdk', 'THKKD_EMSDK_WINDOWS': f'{drive}:/emsdk'}
        run([bash, '--noprofile', '--norc', '/build/build-core.sh'], env=env)
        result = record(build)
        if options.verify:
            previous = json.loads((HERE / 'SOURCE.json').read_text(encoding='utf-8'))
            if previous['files'] != result['files'] or previous['components'] != result['components']:
                raise ValueError('重建产物与登记内容不同')
        else:
            for name in result['files']:
                shutil.copy2(build / 'output' / name, HERE / name)
            (HERE / 'SOURCE.json').write_text(json.dumps(result, indent=2) + '\n', encoding='utf-8', newline='\n')
        print(json.dumps({'result': 'passed', 'verified': options.verify, 'files': result['files']}))
    finally:
        if owned:
            run(['subst', drive + ':', '/d'])


if __name__ == '__main__':
    main()
