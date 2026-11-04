"""Build the managed TH01 core with the hit effects beneath player sprites."""
from pathlib import Path
import argparse
import hashlib
import os
import subprocess
import tempfile

port = Path(__file__).resolve().parents[1]
parser = argparse.ArgumentParser()
parser.add_argument('--source', type=Path, default=port.parents[2] / 'th01-multiorb-lab')
parser.add_argument('--zig', default=os.environ.get('TH01_ZIG', 'zig'))
args = parser.parse_args()
source = args.source.resolve()
core = (source / 'src/core.cpp').read_text(encoding='utf-8')
if hashlib.sha256(core.encode()).hexdigest() != '00e2d192f1525f1c9f6345a64f41bd7effbcd63fa9305f5346013296936416b7':
    raise RuntimeError('TH01 core source differs from the audited layer fix')
start = '    // View ownership changes only this draw order, never the player banks.\n'
effects = '    for(int i=0;i<current_players;++i)render_coop_effect(i);\n'
assert core.count(start) == 1 and core.count(effects) == 1
core = core.replace(effects, '    for(int i=0;i<current_players;++i)if(coop_effect_kind[i]==2)render_coop_effect(i);\n')
core = core.replace(start, '    for(int i=0;i<current_players;++i)if(coop_effect_kind[i]==1||coop_effect_kind[i]==3)render_coop_effect(i);\n' + start)
exports = ['th_init', 'th_set_netplay_rules', 'th_start_stage_entrance', 'th_step', 'th_render', 'th_render_for_slot', 'th_pixels', 'th_render_snapshot_info', 'th_checksum', 'th_net_checksum', 'th_get', 'th_continue', 'th_debug_hit', 'th_debug_position', 'th_debug_invincible', 'th_probe_init', 'th_probe_step', 'th_stage_probe', 'th_stage_command', 'th_sound_event', 'th_layout_probe', 'th_set_difficulty', 'th_set_pause_overlay', 'th_set_bomb_reduced_flash', 'th_menu_open', 'th_menu_input', 'th_score_data', 'th_score_size', 'th_score_load']
output = port / '.cache/layer-core/game.wasm'
output.parent.mkdir(parents=True, exist_ok=True)
with tempfile.TemporaryDirectory(prefix='th01-layer-') as tmp:
    temp = Path(tmp)
    (temp / 'core.cpp').write_text(core, encoding='utf-8', newline='\n')
    cmd = [args.zig, 'c++', '-std=c++17', '-O2', '-fno-exceptions', '-fno-rtti', '-ffp-contract=off', '-fwrapv', '-fno-strict-aliasing', '-Wno-unused-parameter', '-Wno-unused-function', '-Wno-unused-variable', '-Wno-missing-field-initializers', '-Wno-sign-compare', '-Wno-unused-but-set-variable', '-I' + str(source / 'src'), str(temp / 'core.cpp'), '-target', 'wasm32-freestanding', '-ffreestanding', '-nostdlib', '-Wl,--no-entry', '-Wl,--export-memory', '-Wl,--initial-memory=4194304', '-Wl,--max-memory=4194304', '-Wl,-z,stack-size=65536']
    cmd += ['-Wl,--export=' + name for name in exports]
    cmd += ['-o', str(temp / 'game.wasm')]
    subprocess.run(cmd, check=True, cwd=source)
    output.write_bytes((temp / 'game.wasm').read_bytes())
print(output)
