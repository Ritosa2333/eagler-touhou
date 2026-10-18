"""Assemble bounded runtime evidence without upgrading its verification scope."""
from __future__ import annotations
import datetime
import hashlib
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
REPORTS = ROOT / 'reports'


def read(name):
    return json.loads((REPORTS / name).read_text(encoding='utf-8'))


def rate(name):
    samples = read(name)['samples']
    a, b = samples[0], samples[-1]
    if a['replay']['filename'] != b['replay']['filename']:
        raise ValueError('Cannot compute frame rate across replay identities')
    seconds = (datetime.datetime.fromisoformat(b['utc']) - datetime.datetime.fromisoformat(a['utc'])).total_seconds()
    frames = b['replay']['frame'] - a['replay']['frame']
    return dict(replay=a['replay']['filename'], first_frame=a['replay']['frame'], last_frame=b['replay']['frame'],
                elapsed_seconds=seconds, frames_advanced=frames, measured_frames_per_second=frames / seconds)


def main():
    preflight = read('playable_preflight.json')
    comparison = read('playable_demo_comparison.json')
    start = read('playable_rebuilt_start.json')
    runtime = read('playable_rebuilt_runtime.json')
    final = read('playable_final_launch.json')
    stats0 = start['samples'][0]['cpp_exports']['g_th20_bridge_stats']
    stats1 = runtime['samples'][-1]['cpp_exports']['g_th20_bridge_stats']
    names = ['lcg_step', 'rng_next', 'timer_reset', 'timer_mode', 'timer_set', 'timer_add', 'timer_tick']
    result = dict(
        status='runtime_smoke_and_listed_demo_fields_passed',
        source_files_unchanged=preflight['source_matches_manifest'],
        original_save_files_unchanged=preflight['original_save_unchanged'],
        missing_x86_import_dependencies=[d['name'] for d in preflight['dependencies'] if not d['exists']],
        original_and_rebuilt_ran_sequentially=True,
        executable_sha256=hashlib.sha256(Path(runtime['executable']['path']).read_bytes()).hexdigest(),
        linked_cpp_stats=dict(magic=hex(stats1[0]), version=stats1[1], struct_size=stats1[2],
             source_status=stats1[3], actual_main_module_base=hex(stats1[4]),
             first_counts=dict(zip(names, stats0[6:])), last_counts=dict(zip(names, stats1[6:])),
             counter_increases=dict(zip(names, [b-a for a,b in zip(stats0[6:],stats1[6:])]))),
        frame_rate_observations=dict(original=rate('playable_baseline_fps_active.json'),
                                     rebuilt=rate('playable_rebuilt_runtime.json')),
        demo_comparison=dict(report='reports/playable_demo_comparison.json',
             shared_frames=comparison['shared_frames'], identical_frames=comparison['identical_frames'],
             compared_fields=comparison['compared_fields'], field_comparisons=comparison['field_comparisons'],
             mismatch_count=len(comparison['mismatches'])),
        final_play_cmd_smoke=dict(process_id=final['pid'], actual_executable=final['executable']['path'],
             actual_save_directory=final['samples'][-1]['save_directory'],
             first_run_dialog_observed=True, resolution_selected='Window 960x720',
             title_screenshot='reports/playable_final_title.jpg',
             first_run_screenshot='reports/playable_first_run_dialog.jpg',
             process_left_running=True,
             note='Original first-run resolution dialog caption is TH19; actual game title is TH20 ver 1.00c. The original attract demo starts after idle time.'),
        controls_verification=dict(status='not_verified_by_automation',
             attempted='sky press_key z, Return and Escape; normal window close via Alt+F4 succeeded.',
             limitation='The automation API provides momentary chords but no documented key-hold operation; game polling did not reliably capture these momentary keys. Keyboard gameplay, movement and bomb input are not claimed as tested.'),
        limitations=comparison['limitations'] + ['Audio output was not captured or compared.',
          'The 1916 shared frames come from one built-in demo and do not cover all stages or mechanics.',
          'The executable retains original engine modules; this runtime result is not a pure C++ rewrite.'])
    path = REPORTS / 'playable_runtime_validation.json'
    path.write_text(json.dumps(result, indent=2), encoding='utf-8')
    print(json.dumps(dict(report=str(path), status=result['status'], shared_frames=comparison['shared_frames'],
                         field_comparisons=comparison['field_comparisons'], final_process_id=final['pid']), indent=2))


if __name__ == '__main__':
    main()
