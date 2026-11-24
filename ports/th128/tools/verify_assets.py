import argparse
import json
import re
import sys
from pathlib import Path

from th128_assets.formats import FormatError, require
from th128_assets.pipeline import atomic_json, digest, identity, run_tool, snapshot, validate_duplicate_audit, parse_listing, validate_report_destination


PROJECT = Path(__file__).resolve().parents[1]


def verify(local, tools, original=None):
    catalog = json.loads((local / 'catalog.json').read_text(encoding='utf-8'))
    require(catalog['schema'] == 'th128-asset-catalog/1' and catalog['playable'] is False, 'invalid M0 catalog')
    require(digest(tools / 'thanm.exe') == '4ad4a12966a581456db895f7414bd871d9148274497a18dc937b1ce51d977e36',
            'thanm must match official Windows thtk release 12')
    results = {}
    for variant, record in catalog['variants'].items():
        root = (local / record['manifest']).parent
        manifest = json.loads((root / 'manifest.json').read_text(encoding='utf-8'))
        require(manifest['variant'] == variant and manifest['game'] == 'th128', 'variant mismatch')
        for resource in manifest['files']:
            actual = identity(root / 'raw' / resource['name'])
            require(actual == {key: resource[key] for key in ('bytes', 'sha256')}, f'raw resource changed: {resource["name"]}')
        for animation in manifest['animations']:
            metadata = json.loads((root / animation['file']).read_text(encoding='utf-8'))
            text, errors = run_tool(tools / 'thanm.exe', ['-l', root / 'raw' / animation['name']], root)
            require(not errors, f'thanm reported an error: {animation["name"]}: {errors}')
            counts = {'entries': len(re.findall(r'^ENTRY #', text, re.MULTILINE)),
                      'sprites': len(re.findall(r'^Sprite:', text, re.MULTILINE)),
                      'scripts': len(re.findall(r'^Script:', text, re.MULTILINE))}
            for key, count in counts.items():
                require(count == animation[key] == len(metadata[key]), f'independent thanm count mismatch: {animation["name"]} {key}')
            for entry in metadata['entries']:
                if entry['texture']:
                    texture = entry['texture']
                    require(digest(root / texture['file']) == texture['sha256'], 'prepared PNG changed')
        for track in manifest['music']:
            require(digest(root / track['file']) == track['sha256'], 'prepared BGM changed')
        listing_text, _ = run_tool(tools / 'thdat.exe', ['-l128', original / ('th128.dat' if variant == 'ja' else 'th128cn.dat')], root) if original else ('', '')
        if original:
            validate_duplicate_audit(manifest['duplicateEntries'], parse_listing(listing_text))
            for name, expected in manifest['sourceFiles'].items():
                require(identity(original / name) == expected, f'original input changed: {name}')
        proof = json.loads((root.parent / 'source-immutability.json').read_text(encoding='utf-8'))
        require(proof['unchanged'] is True and proof['before'] == proof['after'], 'preparation modified original files')
        if original:
            require(snapshot(original) == proof['before'], 'original installation differs from preparation snapshot')
        results[variant] = {'totals': manifest['totals'], 'rawHashesVerified': len(manifest['files']),
                            'independentlyCheckedAnmArchives': len(manifest['animations']),
                            'pngHashesVerified': manifest['totals']['textures'], 'bgmHashesVerified': len(manifest['music']),
                            'originalUnchanged': True, 'duplicateEntries': manifest['duplicateEntries']}
    return {'milestone': 'M0', 'playable': False, 'variants': results}


def main(argv=None):
    parser = argparse.ArgumentParser(description='Verify actual prepared assets against source hashes and independent thtk listings.')
    parser.add_argument('--local', type=Path, default=PROJECT / 'local')
    parser.add_argument('--thtk', type=Path, required=True)
    parser.add_argument('--original', type=Path)
    parser.add_argument('--report', type=Path, default=PROJECT / 'docs/asset-verification.json')
    arguments = parser.parse_args(argv)
    try:
        protected = [arguments.local.resolve()]
        if arguments.original:
            protected.append(arguments.original.resolve())
        validate_report_destination(arguments.report, protected)
        report = verify(arguments.local.resolve(), arguments.thtk.resolve(), arguments.original.resolve() if arguments.original else None)
        atomic_json(validate_report_destination(arguments.report, protected), report)
        print(json.dumps(report, indent=2))
    except (FormatError, OSError, ValueError, KeyError) as error:
        print(f'Asset verification failed: {error}', file=sys.stderr)
        return 1
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
