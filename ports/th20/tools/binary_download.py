"""Download an official release asset with bounded parallel HTTP ranges.
No installer is executed and files are placed only under the project.
"""
import argparse
import concurrent.futures
import hashlib
import pathlib
import time
import urllib.request

def main():
    p = argparse.ArgumentParser()
    p.add_argument('url')
    p.add_argument('output', type=pathlib.Path)
    p.add_argument('--size', type=int, required=True)
    p.add_argument('--sha256')
    p.add_argument('--workers', type=int, default=16)
    args = p.parse_args()
    args.output.parent.mkdir(parents=True, exist_ok=True)
    parts = args.output.with_suffix(args.output.suffix + '.parts')
    parts.mkdir(exist_ok=True)
    chunk = 1024 * 1024
    count = (args.size + chunk - 1) // chunk
    head = urllib.request.Request(args.url, method='HEAD', headers={'User-Agent':'binary-reconstruction-research'})
    with urllib.request.urlopen(head, timeout=40) as r:
        url = r.url
    start = time.monotonic()
    def part(i):
        first, last = i*chunk, min(args.size, (i+1)*chunk)-1
        out = parts / f'{i:05d}'
        if out.exists() and out.stat().st_size == last-first+1:
            return i
        for attempt in range(5):
            try:
                req=urllib.request.Request(url, headers={'Range':f'bytes={first}-{last}','User-Agent':'binary-reconstruction-research'})
                with urllib.request.urlopen(req, timeout=90) as r:
                    if r.status != 206:
                        raise RuntimeError(f'Range not honored: {r.status}')
                    data=r.read()
                if len(data) != last-first+1:
                    raise RuntimeError(f'Wrong range size: {len(data)}')
                out.write_bytes(data)
                return i
            except Exception as e:
                if attempt == 4:
                    raise
                print(f'retry {i}: {e}', flush=True)
                time.sleep(2+attempt)
    with concurrent.futures.ThreadPoolExecutor(max_workers=args.workers) as pool:
        for n, future in enumerate(concurrent.futures.as_completed([pool.submit(part,i) for i in range(count)]), 1):
            i=future.result()
            print(f'{n}/{count} parts; {time.monotonic()-start:.1f}s; part {i}', flush=True)
    h=hashlib.sha256()
    with args.output.open('wb') as w:
        for i in range(count):
            data=(parts/f'{i:05d}').read_bytes()
            w.write(data)
            h.update(data)
    digest=h.hexdigest()
    print(f'SHA256={digest}',flush=True)
    if args.sha256 and digest.lower()!=args.sha256.lower():
        raise RuntimeError('SHA256 mismatch')

if __name__=='__main__':
    main()
