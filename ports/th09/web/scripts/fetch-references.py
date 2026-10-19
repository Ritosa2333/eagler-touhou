"""Fetch pinned, public format/address references. Never executes downloaded code."""
from pathlib import Path
import urllib.request, json, base64, hashlib

root = Path(__file__).resolve().parents[1] / 'reference/upstream'
items = [
    ('thpatch/thtk', '892114a0fcaa0bbdaaecf3cb4ad56f758683fb40', ['COPYING', 'thtk/thdat08.c', 'thtk/thcrypt.c', 'thtk/thcrypt.h']),
    ('touhouworldcup/thprac', '585fae1aad4b720f350655a44e3f2ec7fc0dfa25', ['LICENCE', 'thprac/src/thprac/thprac_th09.cpp']),
]
manifest = []
for repo, commit, files in items:
    for name in files:
        req = urllib.request.Request(f'https://api.github.com/repos/{repo}/contents/{name}?ref={commit}', headers={'User-Agent': 'TH09-Reconstruction'})
        with urllib.request.urlopen(req, timeout=25) as response:
            data = json.load(response)
        content = base64.b64decode(data['content'])
        dest = root / repo.split('/')[-1] / name
        dest.parent.mkdir(parents=True, exist_ok=True)
        dest.write_bytes(content)
        manifest.append({'repository':'https://github.com/'+repo, 'commit':commit, 'path':name, 'sha256':hashlib.sha256(content).hexdigest(), 'use':'Reference only; not linked into the game'})
        print(name, len(content), flush=True)
(root/'sources.json').write_text(json.dumps(manifest, indent=2)+'\n', 'utf-8')
