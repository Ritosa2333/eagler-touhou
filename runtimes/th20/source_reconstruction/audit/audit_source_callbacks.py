"""Bounded read-only callback audit; does not execute original game code.

Enumerates the original registration callers, captures the corresponding current
production source and selected return-value evidence, and hashes every input.
This is an evidence collector, not a whole-program equivalence test.
"""
from __future__ import annotations

import hashlib
import json
from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / '.cache/binary_python'))
import capstone
import pefile

ORIGINAL = Path(r'E:\BaiduNetdiskDownload\PCGAME-Touhou.Kinjoukyou.Fossilized.Wonders-P2P\Touhou.Kinjoukyou.Fossilized.Wonders\th20.exe')
EXPECTED_SHA = 'a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897'
SOURCE = ROOT / 'source_reconstruction'
OWNERS = {
    '00423bf0': ['screen_effect/lifecycle.cpp'],
    '00447e00': ['sprite_renderer/controller.cpp', 'sprite_renderer/controller_callbacks.inc'],
    '0046c080': ['text_renderer/text.cpp'],
    '00473160': ['stage_background/lifecycle.cpp'],
    '00477dc0': ['bomb_system/controller.cpp'],
    '00480fa0': ['bullet_system/controller.cpp'],
    '00487b10': ['card_system/lifecycle.cpp'],
    '0049d790': ['effect_system/lifecycle.cpp'],
    '004a0600': ['ending_scene/resources.cpp'],
    '004a7080': ['gameplay/enemy.cpp'],
    '004ac080': ['platform_window/frame_statistics.cpp'],
    '004b5820': ['hud_system/lifecycle.cpp'],
    '004bad40': ['gameplay/loading_adapter.cpp'],
    '004bf990': ['help_system/help.cpp'],
    '004c0b30': ['damage_regions/controller.cpp'],
    '004c3c00': ['item_system/state.cpp'],
    '004c58d0': ['key_config/lifecycle.cpp'],
    '004d3ca0': ['laser_system/controller.cpp'],
    '004d8220': ['startup_scene/startup.cpp'],
    '004de1f0': ['platform_window/graphics_callbacks.cpp'],
    '004df670': ['notice_system/notice.cpp'],
    '004e0e80': ['options_system/lifecycle.cpp'],
    '004e5980': ['pause_system/lifecycle.cpp'],
    '004f9520': ['player_entity/initialize.cpp'],
    '00508480': ['replay_system/initialize.cpp'],
    '00510640': ['small_score/state.cpp'],
    '00511270': ['stage_clear/lifecycle.cpp'],
    '00519960': ['stone_menu/lifecycle.cpp'],
    '0051f3c0': ['title_system/lifecycle_core.cpp', 'title_system/lifecycle.cpp'],
    '0052e170': ['trophy_system/lifecycle.cpp'],
    '00532fb0': ['overlay_system/lifecycle.cpp'],
}


def digest(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def main() -> None:
    data = ORIGINAL.read_bytes()
    assert digest(data) == EXPECTED_SHA, 'Unexpected original executable'
    pe = pefile.PE(data=data)
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    pseudo_path = SOURCE / 'audit/compile_probe/th20_pseudocode_unmodified.cpp'
    pseudo = pseudo_path.read_text(encoding='utf-8', errors='replace')
    units = {}
    for text in re.split(r'(?=/\* Entry:)', pseudo):
        match = re.search(r'/\* Entry: 0x([0-9a-f]+);.*?body bytes: (\d+)', text)
        if match:
            units[match[1]] = (text, int(match[2]))
    registration = re.compile(r'FUN_(00412(?:2c0|310|360|3b0))\(([^,]+),FUN_(00[0-9a-f]+),([^;]+)\);')

    def asm(address: str) -> list[str]:
        va = int(address, 16)
        size = units[address][1]
        return [f'{i.address:08x} {i.mnemonic} {i.op_str}'.rstrip()
                for i in md.disasm(pe.get_data(va - pe.OPTIONAL_HEADER.ImageBase, size), va)]

    owners = []
    hashes = {}
    callbacks = set()
    for address, (text, _) in units.items():
        calls = registration.findall(text)
        if not calls:
            continue
        assert address in OWNERS, f'Unmapped registration owner: {address}'
        sources = []
        for relative in OWNERS[address]:
            path = SOURCE / relative
            raw = path.read_bytes()
            hashes[relative] = digest(raw)
            lines = raw.decode('utf-8').splitlines()
            sources.append({'file': relative, 'registration_lines': [
                {'line': n, 'text': line.strip()} for n, line in enumerate(lines, 1)
                if 'register_callback(' in line or 'enable_callbacks();' in line
            ]})
        records = []
        for helper, priority, callback, owner in calls:
            callbacks.add(callback)
            records.append({'helper': helper, 'priority_expression': priority,
                            'callback': callback, 'owner_expression': owner,
                            'draw': helper in ('00412360', '004123b0'),
                            'initially_enabled': helper in ('004122c0', '00412360')})
        owners.append({'original_owner': address, 'registrations': records,
                       'production_sources': sources, 'original_assembly': asm(address)})

    # Void decompilations whose literal return matters to scheduler retention;
    # include genuine empty virtual functions to prevent false stub reports.
    selected = ['004122c0', '00412310', '00412360', '004123b0',
                '004216a0', '00424e00', '00424e80', '00424f30', '00424fa0',
                '004dd1c0', '004dd2e0', '00478bf0', '00412540', '00477ce0',
                '0040e5e0', '00414b60', '0052fbc0', '004ba870', '004b5790',
                '00470300', '0046b6e0']
    report = {
        'status': 'bounded_static_evidence_not_whole_game_validation',
        'original_sha256': EXPECTED_SHA,
        'pseudocode_index_sha256': digest(pseudo_path.read_bytes()),
        'source_sha256': hashes,
        'original_owner_count': len(owners),
        'registration_callsite_count': sum(len(o['registrations']) for o in owners),
        'distinct_registered_callback_count': len(callbacks),
        'owners': owners,
        'selected_return_and_empty_function_evidence': {address: asm(address) for address in selected},
        'limits': ['Original direct registration callers indexed by existing Ghidra evidence.',
                   'Source snippets require review; this collector does not parse C++ or prove callback order.',
                   'Loop registrations in Sprite/Graphics are recorded in their source table.',
                   'Production graph reachability and whole-frame equivalence require separate validation.'],
    }
    output = SOURCE / 'audit/source_game_callbacks.json'
    output.write_text(json.dumps(report, indent=2, ensure_ascii=False) + '\n', encoding='utf-8')
    print(json.dumps({k: report[k] for k in ('original_owner_count', 'registration_callsite_count',
                                          'distinct_registered_callback_count')}))


if __name__ == '__main__':
    main()
