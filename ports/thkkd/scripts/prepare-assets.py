"""从原字节资源可重建浏览器图像、OGG、WebM 和安装用 DATA。"""
from pathlib import Path
import argparse
import hashlib
import json
import os
import shutil
import struct
import subprocess
from PIL import Image, ImageChops

ROOT = Path(__file__).resolve().parents[1]

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output', type=Path, default=ROOT / 'build/web-content')
    parser.add_argument('--ffmpeg', default='ffmpeg')
    args = parser.parse_args()
    output = args.output.resolve()
    converted = ROOT / 'build/web-assets'
    converted.mkdir(parents=True, exist_ok=True)
    output.mkdir(parents=True, exist_ok=True)
    manifest = []
    sources = sorted((ROOT / 'assets').rglob('*'))
    cache_path = converted / 'conversion-cache.json'
    cache = json.loads(cache_path.read_text('utf-8')) if cache_path.exists() else {}
    times_path = ROOT.parents[1] / 'launcher/src/launcher/thkkd-original-assets.json'
    resource_times = json.loads(times_path.read_text('utf-8'))['files'] if times_path.exists() else {}
    next_cache = {}
    for source in sources:
        if not source.is_file():
            continue
        name = source.relative_to(ROOT / 'assets').as_posix()
        suffix = source.suffix.lower()
        if source.stem.endswith('_a') and suffix in ('.png', '.bmp', '.jpg'):
            continue
        target = converted / name
        recipe = 'copy'
        mask = source.with_name(source.stem + '_a' + suffix)
        if suffix in ('.png', '.bmp', '.jpg'):
            target = target.with_name(target.name + '.png')
            recipe = 'rgba-mask-v2'
        elif suffix == '.mp3':
            target = target.with_suffix('.ogg')
            recipe = 'libvorbis-q6-v1'
        elif suffix == '.wmv':
            target = target.with_suffix('.webm')
            recipe = 'libvpx-crf10-v1'
        digest = hashlib.sha256(source.read_bytes()).hexdigest()
        original = resource_times.get(name)
        if original and original['sha256'] == digest:
            os.utime(source, ns=(source.stat().st_atime_ns, original['mtimeNs']))
        mask_digest = hashlib.sha256(mask.read_bytes()).hexdigest() if mask.exists() else None
        identity = {'sourceSha256': digest, 'maskSha256': mask_digest, 'recipe': recipe}
        if cache.get(name) != identity or not target.exists():
            target.parent.mkdir(parents=True, exist_ok=True)
            if recipe == 'rgba-mask-v2':
                with Image.open(source) as image:
                    rgba = image.convert('RGBA')
                    if mask.exists():
                        with Image.open(mask) as alpha:
                            rgba.putalpha(ImageChops.multiply(rgba.getchannel('A'), alpha.convert('RGB').getchannel('R')))
                    rgba.save(target)
            elif suffix == '.mp3':
                subprocess.run([args.ffmpeg, '-hide_banner', '-loglevel', 'error', '-y', '-i', str(source), '-map_metadata', '0', '-c:a', 'libvorbis', '-q:a', '6', str(target)], check=True)
            elif suffix == '.wmv':
                subprocess.run([args.ffmpeg, '-hide_banner', '-loglevel', 'error', '-y', '-i', str(source), '-map_metadata', '0', '-c:v', 'libvpx', '-crf', '10', '-b:v', '2M', '-deadline', 'good', '-cpu-used', '4', '-an', str(target)], check=True)
            else:
                shutil.copy2(source, target)
            print(name, flush=True)
        os.utime(target, ns=(source.stat().st_atime_ns, source.stat().st_mtime_ns))
        next_cache[name] = identity
        info = {'source': name, 'file': target.relative_to(converted).as_posix(), **identity,
                'bytes': target.stat().st_size, 'sha256': hashlib.sha256(target.read_bytes()).hexdigest()}
        if recipe == 'rgba-mask-v2':
            with Image.open(source) as image:
                info['intrinsicAlpha'] = 'A' in image.getbands() or 'transparency' in image.info or mask.exists()
        manifest.append(info)
    cache_path.write_text(json.dumps(next_cache, indent=2), encoding='utf-8')
    offset = 0
    for row in manifest:
        row['offset'] = offset
        offset += row['bytes']
    header = json.dumps({'schema': 'thkkd/web-assets/1', 'files': manifest}, separators=(',', ':')).encode('utf-8')
    with (output / 'thkkd.data').open('wb') as target:
        target.write(b'THKKD1\0\0' + struct.pack('<I', len(header)) + header)
        for row in manifest:
            with (converted / row['file']).open('rb') as source:
                shutil.copyfileobj(source, target, 1024 * 1024)
    (output / 'provenance.json').write_text(json.dumps({'schema': 'thkkd/web-assets/1', 'files': manifest}, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
    print(json.dumps({'files': len(manifest), 'bytes': (output / 'thkkd.data').stat().st_size}), flush=True)

if __name__ == '__main__':
    main()
