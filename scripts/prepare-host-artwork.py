"""Compose TH12 Host artwork from decoded title.anm layers, without a VM.

This is a static title-art derivative. The first script block's position and
opacity interpolation endpoints describe the settled title artwork; interrupt
branches, menu rows, version/copyright text and transition gradients are not
part of the Host card. Native title binding:43fe6b..43fe7c (scripts99/106).
"""
import hashlib
import json
import math
import os
import pathlib

from PIL import Image

ROOT = pathlib.Path(__file__).resolve().parents[1]
ASSETS = pathlib.Path(os.environ.get('TH12_ASSETS', ROOT / 'site/assets')).expanduser().resolve()
ARTWORK = pathlib.Path(os.environ.get('TH12_ARTWORK', ROOT / 'artifacts/eagler-content/artwork')).expanduser().resolve()
PINNED_EXE_SHA256 = '99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417'
# The ordering follows the retail layers: sky21, right title tile21, left
# title tile22, then logo24. The two tiles in each pair do not overlap.
TITLE_LAYERS = [(102, 68), (103, 69), (101, 67), (100, 66), (108, 70)]


def settled_art_pose(atlas, script_id, sprite_id):
    """Read literal endpoints from the initial block, never execute ANM code."""
    pose = {'script': script_id, 'sprite': None, 'x': 0, 'y': 0, 'layer': 0,
            'anchorX': 0, 'anchorY': 0, 'alpha': 255}
    stopped = False
    for command in atlas['scripts'][str(script_id)]['commands']:
        opcode, integers, floats = command['op'], command['ints'], command['floats']
        if opcode == 63:
            stopped = True
            break
        if command['mask']:
            raise ValueError(f'Title card script{script_id} requires a nonliteral parameter')
        if opcode == 3:
            pose['sprite'] = integers[0]
        elif opcode == 48:
            pose['x'], pose['y'] = floats[:2]
        elif opcode == 56:
            # End of the initial slide-in, before the first interrupt stop.
            pose['x'], pose['y'] = floats[2:4]
        elif opcode == 65:
            pose['anchorX'], pose['anchorY'] = integers[0] & 3, (integers[0] >> 16) & 3
        elif opcode == 68:
            pose['layer'] = integers[0]
        elif opcode == 51:
            pose['alpha'] = integers[0] & 255
        elif opcode == 58:
            pose['alpha'] = integers[2] & 255
        elif opcode in (49, 50, 52, 53, 54, 57, 59, 60, 61, 62, 66, 76, 78, 79, 80, 100):
            raise ValueError(f'Title card script{script_id} has an unsupported artwork transform')
    if not stopped or pose['sprite'] != sprite_id or pose['alpha'] != 255:
        raise ValueError(f'Unexpected initial title-art block: script{script_id}')
    if not all(isinstance(pose[key], (int, float)) and math.isfinite(pose[key]) for key in ('x', 'y')):
        raise ValueError('Title artwork has an invalid placement')
    return pose


def main():
    atlas = json.loads((ASSETS / 'atlases.json').read_text('utf-8'))['title']
    if atlas['provenance']['originalExeSha256'] != PINNED_EXE_SHA256:
        raise ValueError('Host artwork requires the TH12 1.00b title metadata')
    canvas = Image.new('RGBA', (640, 480), (0, 0, 0, 0))
    poses = [settled_art_pose(atlas, script, sprite) for script, sprite in TITLE_LAYERS]
    layers = []
    for pose in sorted(poses, key=lambda item: item['layer']):
        sprite = atlas['sprites'][str(pose['sprite'])]
        entry = atlas['entries'][sprite['entry']]
        filename = entry['image']
        if not isinstance(filename, str) or pathlib.Path(filename).name != filename:
            raise ValueError('Title artwork texture must be a decoded atlas PNG')
        coordinates = [sprite[key] for key in ('x', 'y', 'w', 'h')]
        if any(not math.isfinite(value) or value != int(value) for value in coordinates):
            raise ValueError('Title artwork sprite has fractional source coordinates')
        x, y, width, height = map(int, coordinates)
        with Image.open(ASSETS / filename) as image:
            if width <= 0 or height <= 0 or x < 0 or y < 0 or x + width > image.width or y + height > image.height:
                raise ValueError('Title artwork sprite exceeds its atlas texture')
            tile = image.convert('RGBA').crop((x, y, x + width, y + height))
        left = pose['x'] - (0 if pose['anchorX'] == 1 else width if pose['anchorX'] == 2 else width / 2)
        top = pose['y'] - (0 if pose['anchorY'] == 1 else height if pose['anchorY'] == 2 else height / 2)
        # Script positions already include the512-pixel right-tile offset;
        # adding the ANM entry xOffset again would duplicate that offset.
        destination = (math.floor(left), math.floor(top))
        canvas.alpha_composite(tile, destination)
        layers.append({**pose, 'source': entry['name'], 'texture': filename,
                       'crop': [x, y, width, height], 'destination': list(destination)})
    ARTWORK.mkdir(parents=True, exist_ok=True)
    target = ARTWORK / 'th12-card.webp'
    canvas.convert('RGB').save(target, format='WEBP', quality=55, method=6)
    metadata = {'schema': 'th12-host-artwork-source/1', 'game': 'th12', 'file': target.name,
                'width': 640, 'height': 480, 'source': 'title.anm',
                'sourceSha256': atlas['provenance']['sha256'], 'originalExeSha256': PINNED_EXE_SHA256,
                'composition': 'settled-initial-title-artwork', 'layers': layers,
                'excluded': ['menu rows', 'version', 'copyright', 'transition gradients'],
                'webp': {'quality': 55, 'method': 6},
                'sha256': hashlib.sha256(target.read_bytes()).hexdigest(), 'hostOnly': True}
    (ARTWORK / 'th12-card.source.json').write_text(json.dumps(metadata, ensure_ascii=False, indent=2) + '\n', 'utf-8')
    print(f'Prepared Host title artwork: {target}')


if __name__ == '__main__':
    main()
