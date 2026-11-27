"""完整解码两种视频内核的输出，区分固定时钟一致性与真实时钟性能。"""
import argparse
from concurrent.futures import ThreadPoolExecutor
import hashlib
import json
from pathlib import Path
import re
import subprocess


def read(path):
    return json.loads(path.read_text('utf-8'))


def decoded(path):
    result = subprocess.check_output(['ffmpeg', '-v', 'error', '-i', str(path),
        '-map', '0:v:0', '-an', '-pix_fmt', 'yuv420p', '-fps_mode', 'passthrough', '-f', 'framemd5', '-'])
    path.with_suffix('.framemd5').write_bytes(result)
    return [line for line in result.decode().splitlines() if not line.startswith('#')]


def container_uids(before, after):
    normalized = [bytearray(before), bytearray(after)]
    result = {}
    for name, marker in [('TrackUID', bytes.fromhex('73c588')), ('TagTrackUID', bytes.fromhex('63c588'))]:
        fields = []
        for data in normalized:
            at = data.find(marker, 0, 4096)
            assert at >= 0
            start = at + len(marker)
            fields.append({'offset': start, 'value': data[start:start+8].hex()})
            data[start:start+8] = bytes(8)
        result[name] = {'baseline': fields[0], 'candidate': fields[1]}
    assert normalized[0] == normalized[1], '固定时钟 WebM 在随机轨道标识以外存在差异'
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--real', type=Path, required=True)
    parser.add_argument('--fixed', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    # 转码测量在生命周期检查前独立落盘，最终生命周期使用修订后的正式检查结果。
    real = read(args.real/'progress.json')
    fixed = read(args.fixed/'result.json')
    assert real.get('clock', 'real') == 'real' and fixed['clock'] == 'fixed-20ms'
    assert fixed['result'] == 'passed' and all(fixed['lifecycle'].values())
    assert real['inputs'] == fixed['inputs']
    capabilities = fixed['variants']['candidate']['capabilities']
    patterns = {
        'decoders': (r'^ [VAS][.A-Z]{5} ([a-z][a-z0-9_]+)\s', ['h263', 'vc1', 'wmv3']),
        'encoders': (r'^ [VAS][.A-Z]{5} ([a-z][a-z0-9_]+)\s', ['libvpx']),
        'demuxers': (r'^ D  ([a-z][a-z0-9_]+)\s', ['asf']),
        'muxers': (r'^  E ([a-z][a-z0-9_]+)\s', ['webm']),
        'protocols': (r'^  ([a-z][a-z0-9]+)$', ['file', 'file']),
        'filters': (r'^ [.TSC]{3} ([a-z][a-z0-9_]+)\s', ['format', 'null', 'scale', 'abuffer', 'buffer', 'abuffersink', 'buffersink']),
    }
    actual = {}
    for kind, (pattern, expected) in patterns.items():
        actual[kind] = re.findall(pattern, capabilities[kind], re.M)
        assert sorted(actual[kind]) == sorted(expected), (kind, actual[kind])
    for variant in ['baseline', 'candidate']:
        assert real['variants'][variant]['wasmSha256'] == fixed['variants'][variant]['wasmSha256']
        assert len(real['variants'][variant]['files']) == 4
    report = {'schema': 'thkkd/video-comparison/1', 'clockComparison': 'fixed-20ms',
        'inputs': real['inputs'], 'variants': real['variants'], 'actualCapabilities': actual, 'lifecycle': fixed['lifecycle'], 'videos': []}
    with ThreadPoolExecutor(max_workers=2) as pool:
        for name in ['back100', 'back200', 'back201', 'back300']:
            old, new = list(pool.map(decoded, [args.fixed/v/(name+'.webm') for v in ['baseline', 'candidate']]))
            assert old == new, name + ' 固定时钟解码帧或时间戳不同'
            before, after = list(pool.map(decoded, [args.real/v/(name+'.webm') for v in ['baseline', 'candidate']]))
            assert [row.rsplit(',', 1)[0] for row in before] == [row.rsplit(',', 1)[0] for row in after]
            metadata = []
            for variant in ['baseline', 'candidate']:
                raw = subprocess.check_output(['ffprobe', '-v', 'error', '-show_format', '-show_streams', '-of', 'json', str(args.fixed/variant/(name+'.webm'))])
                info = json.loads(raw)
                stream = info['streams'][0]
                assert len(info['streams']) == 1 and stream['codec_name'] == 'vp8'
                assert (stream['width'], stream['height']) == (384, 448)
                metadata.append({'duration': info['format']['duration'], 'tags': info['format'].get('tags'), 'streamTags': stream.get('tags')})
            quality = subprocess.run(['ffmpeg', '-hide_banner', '-i', str(args.real/'baseline'/(name+'.webm')),
                '-i', str(args.real/'candidate'/(name+'.webm')), '-lavfi', '[0:v][1:v]ssim', '-an', '-f', 'null', '-'], capture_output=True, text=True, check=True)
            ssim = float(re.findall(r'All:([0-9.]+)', quality.stderr)[-1])
            containers = [(args.fixed/v/(name+'.webm')).read_bytes() for v in ['baseline', 'candidate']]
            report['videos'].append({'name': name, 'frames': len(new), 'fixedPixelsAndTimestampsEqual': True,
                'fixedDecodedSha256': hashlib.sha256('\n'.join(new).encode()).hexdigest(),
                'realTimestampsEqual': True, 'realOldVsNewSSIM': ssim,
                'fixedContainerBytesEqual': containers[0] == containers[1],
                'fixedContainerUIDs': container_uids(*containers), 'fixedContainerEqualExceptTrackUIDs': True,
                'containerMetadata': {'baseline': metadata[0], 'candidate': metadata[1]}})
    totals = {v: sum(row['milliseconds'] for row in real['variants'][v]['files']) for v in ['baseline', 'candidate']}
    report['realTotalMilliseconds'] = totals
    report['realTimingRatio'] = totals['candidate']/totals['baseline']
    assert report['realTimingRatio'] <= 1.1
    assert real['variants']['candidate']['wasmBytes'] <= 8*1024*1024
    assert real['variants']['candidate']['gzipBytes'] <= 3*1024*1024
    report['memoryMetric'] = 'Worker WASM linear memory capacity high-water mark; MEMFS and browser process memory recorded separately when sampled'
    report['result'] = 'passed'
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, ensure_ascii=False, indent=2)+'\n', 'utf-8')
    print(json.dumps({'result': 'passed', 'frames': sum(v['frames'] for v in report['videos']), 'timingRatio': report['realTimingRatio']}))


if __name__ == '__main__':
    main()
