"""Run real TH08 diagnostic captures in one isolated browser context.

Evidence goes to the ignored artifacts directory, never the launcher or retail saves.
The classifier unit suite is separate: these checks must actually boot the WASM.
"""
import argparse
import hashlib
import json
from datetime import datetime, timezone
from pathlib import Path
from uuid import uuid4
from playwright.sync_api import sync_playwright

ROOT = Path(__file__).resolve().parents[2]
EVIDENCE_ROOT = ROOT / 'artifacts/presentation-lab/evidence'

SAMPLE = """({label, negativeControl=false}) => {
  const c=presentationLab.controller, r=c.sweep({label,negativeControl});
  presentationLab.renderReport(r);
  return r;
}"""


def brief(report):
    return {k: report[k] for k in ('label', 'tick', 'valid', 'purityStatus', 'purity', 'gate', 'counts', 'coverage', 'stateChanges', 'stateAuthority')}


def check_row(adaptation_id, status, complete, machine_complete=None, **evidence):
    return {'id': adaptation_id, 'status': status, 'complete': bool(complete),
            'machineComplete': bool(complete if machine_complete is None else machine_complete),
            'evidence': {'game': 'th08', 'variant': 'normal', 'adaptationIds': [adaptation_id], **evidence}}


def report_objects(report):
    return report.get('objects', []) if isinstance(report, dict) else []


def interpolated_position_owners(report):
    result = []
    for obj in report_objects(report):
        if any(field.get('id') == 'position' and field.get('status') == 'interpolated'
               for field in obj.get('fields', [])):
            result.append({'ownerId': obj.get('ownerId'), 'owner': obj.get('owner'),
                           'object': obj.get('object'), 'generation': obj.get('generation')})
    return result


def lifecycle_delta(before, after):
    if not isinstance(before, dict) or not isinstance(after, dict):
        return {'available': False, 'reason': 'lifecycle snapshots are missing', 'events': []}
    if not before.get('available') or not after.get('available'):
        return {'available': False, 'reason': 'native lifecycle ring is unavailable', 'events': []}
    if before.get('capacity') != after.get('capacity') or before.get('dropped') != 0 or after.get('dropped') != 0:
        return {'available': False, 'reason': 'lifecycle ring capacity changed or overflow was recorded', 'events': []}
    old, new = before.get('events'), after.get('events')
    if not isinstance(old, list) or not isinstance(new, list) or len(new) < len(old) or new[:len(old)] != old:
        return {'available': False, 'reason': 'lifecycle snapshots do not retain an unambiguous prefix', 'events': []}
    return {'available': True, 'reason': None, 'events': new[len(old):],
            'capacity': after['capacity'], 'dropped': after['dropped']}


def pool_reuse_boundary(capture, lifecycle_events):
    if not isinstance(capture, dict):
        return None, False
    report, event = capture.get('report'), capture.get('event')
    if not isinstance(report, dict) or not isinstance(event, dict):
        return {'status': 'incomplete', 'reason': 'missing raw current-state capture or lifecycle pair'}, False
    retired, born = event.get('retired'), event.get('born')
    fields = (retired, born)
    if not all(isinstance(item, dict) for item in fields):
        return {'status': 'incomplete', 'reason': 'missing retire/birth event pair'}, False
    same_owner_object = retired.get('owner') == born.get('owner') and retired.get('object') == born.get('object')
    newer_generation = (isinstance(retired.get('generation'), int) and
                        isinstance(born.get('generation'), int) and born['generation'] > retired['generation'])
    retired_index = next((index for index, item in enumerate(lifecycle_events) if item == retired), None)
    born_index = next((index for index, item in enumerate(lifecycle_events) if item == born), None)
    ordered = (retired.get('operation') == 2 and born.get('operation') == 1 and
               retired_index is not None and born_index is not None and retired_index < born_index)
    pair_retained = retired_index is not None and born_index is not None
    owner, object_id = born.get('owner'), born.get('object')
    old_prefix = f'{owner}:{object_id}:{retired.get("generation")}:'
    old = next((item for item in report.get('disappeared', [])
                if item.get('ownerId') == owner and item.get('generation') == retired.get('generation') and
                isinstance(item.get('key'), str) and item['key'].startswith(old_prefix)), None)
    current = next((item for item in report_objects(report)
                    if item.get('ownerId') == owner and item.get('object') == object_id and
                    item.get('generation') == born.get('generation')), None)
    position_interpolated = bool(current and any(
        field.get('id') in {'position', 'screen-position'} and field.get('status') == 'interpolated'
        for field in current.get('fields', [])))
    capture_valid = (report.get('schema') == 'th08/presentation-audit/1' and
                     report.get('label') == 'display05-pool-reuse-boundary' and report.get('valid') is True and
                     report.get('dropped') == 0 and report.get('lifecycle', {}).get('status') == 'complete' and
                     report.get('lifecycle', {}).get('dropped') == 0)
    ok = bool(capture_valid and same_owner_object and newer_generation and ordered and pair_retained and old and
              old.get('status') == 'lifecycle' and current and current.get('status') == 'unobserved' and
              current.get('identityFlags') == 3 and not position_interpolated)
    boundary = {
        'status': 'passed' if ok else 'incomplete',
        'event': event,
        'reportLabel': report.get('label'),
        'reportTick': report.get('tick'),
        'oldGenerationState': ({'key': old.get('key'), 'generation': old.get('generation'),
                                'status': old.get('status')} if old else None),
        'newGenerationState': ({'key': current.get('key'), 'generation': current.get('generation'),
                                'identityFlags': current.get('identityFlags'), 'status': current.get('status')} if current else None),
        'positionInterpolated': position_interpolated,
        'rawReportSchema': report.get('schema'),
        'rawReportValid': report.get('valid') is True,
        'rawLifecycleStatus': report.get('lifecycle', {}).get('status'),
        'retireAndBirthRetainedInWindow': pair_retained,
    }
    return boundary, ok


def scene_snapshot(report):
    if not isinstance(report, dict):
        return None
    scene = report.get('scene')
    if not isinstance(scene, dict):
        return None
    diagnostics, status = scene.get('diagnostics'), scene.get('status')
    if not isinstance(diagnostics, list) or len(diagnostics) != 16 or not isinstance(status, list) or len(status) < 10:
        return None
    return {'activeScene': diagnostics[0], 'targetScene': diagnostics[1], 'previousScene': diagnostics[2],
            'loading': diagnostics[3], 'supervisorActive': status[0], 'stage': status[1],
            'runtimeError': status[2], 'lives': status[3]}


def scene_transition_evidence(title_report, game_report):
    before, after = scene_snapshot(title_report), scene_snapshot(game_report)
    ok = bool(before and after and before['activeScene'] == 1 and before['runtimeError'] == 0 and
              after['activeScene'] == 2 and after['supervisorActive'] == 2 and
              after['runtimeError'] == 0 and after['loading'] == 0)
    return {'status': 'passed' if ok else 'incomplete', 'from': before, 'to': after,
            'source': 'BrowserExports diagnostics[0] active Scene Title=1 -> Game=2; diagnostics[3] loading_game; sdl_game_status[0..2] active/stage/error'}, ok


def display_observation_report(reports, lifecycle_before, lifecycle_after, first_frame, pool_reuse=None):
    by_label = {item.get('label'): item for item in reports}
    motion = by_label.get('stage1-motion')
    pause = by_label.get('pause-menu')
    resumed = by_label.get('pause-resumed')
    positions = interpolated_position_owners(motion)
    position_owners = sorted({item['ownerId'] for item in positions if isinstance(item.get('ownerId'), int)})
    player_interpolated = any(item['ownerId'] == 13 for item in positions)
    other_interpolated = any(item['ownerId'] in {5, 6, 7, 8, 9, 11, 14, 16, 17, 19, 20, 21}
                             for item in positions)
    motion_status = ('failed' if motion and (not motion.get('valid') or motion.get('purityStatus') != 'pass') else
                     'passed' if motion and player_interpolated and other_interpolated else 'incomplete')

    delta = lifecycle_delta(lifecycle_before, lifecycle_after)
    events = delta['events']
    births = [event for event in events if event.get('operation') == 1]
    retires = [event for event in events if event.get('operation') == 2]
    lifecycle_errors = [event for event in events if event.get('operation') in {3, 4, 5}]
    reuse = []
    retire_history = {}
    for index, event in enumerate(events):
        if event.get('operation') == 2:
            retire_history.setdefault((event.get('owner'), event.get('object')), []).append((index, event))
    for index, event in enumerate(events):
        if event.get('operation') != 1:
            continue
        key = (event.get('owner'), event.get('object'))
        prior = [(old_index, old) for old_index, old in retire_history.get(key, [])
                 if old_index < index and isinstance(old.get('generation'), int) and
                 isinstance(event.get('generation'), int) and event['generation'] > old['generation']]
        if prior:
            old_index, old = prior[-1]
            reuse.append({'owner': key[0], 'object': key[1], 'retiredGeneration': old.get('generation'),
                          'bornGeneration': event.get('generation'), 'retireIndex': old_index, 'birthIndex': index})
    title_before = by_label.get('display05-title-before')
    scene_after = by_label.get('stage1-motion')
    scene_transition, scene_cut_ok = scene_transition_evidence(title_before, scene_after)

    pool_boundary, pool_reuse_ok = pool_reuse_boundary(pool_reuse, events)
    lifecycle_machine_ok = bool(delta['available'] and births and retires and reuse and scene_cut_ok and pool_reuse_ok)
    lifecycle_status = ('failed' if lifecycle_errors else
                        'passed' if lifecycle_machine_ok else 'incomplete')

    paused_objects = [obj for obj in report_objects(pause) if obj.get('ownerId') == 3]
    pause_ok = bool(pause and pause.get('valid') and pause.get('worldFrozen') and
                    pause.get('counts', {}).get('frozen', 0) > 0 and paused_objects)
    resume_ok = bool(resumed and resumed.get('valid') and not resumed.get('worldFrozen'))
    pause_status = 'passed' if pause_ok and resume_ok else 'incomplete'

    first_frame_ok = bool(isinstance(first_frame, dict) and first_frame.get('event') == 'first-frame' and
                          isinstance(first_frame.get('epoch'), int) and
                          first_frame.get('epoch') == first_frame.get('runtimeEpoch') and
                          isinstance(first_frame.get('nativePresentCountAtReceipt'), int) and
                          first_frame['nativePresentCountAtReceipt'] > 0 and
                          first_frame.get('origin') == first_frame.get('expectedOrigin'))
    rows = [
        check_row('DISPLAY-04', motion_status, False, machine_complete=motion_status == 'passed', reportLabel='stage1-motion',
                  tick=motion.get('tick') if motion else None, interpolatedPositionOwners=position_owners,
                  requiredOwnerClasses=['player', 'projectile', 'laser', 'familiar', 'animation-transition'],
                  automatedScope='captured owner position fields and valid interpolation window'),
        check_row('DISPLAY-05', lifecycle_status, False, machine_complete=lifecycle_machine_ok, ringAvailable=delta['available'],
                  ringReason=delta['reason'], eventCount=len(events), births=len(births), retires=len(retires),
                  recycledPointerGenerations=reuse, invalidEvents=lifecycle_errors,
                  sceneTransition=scene_transition,
                  poolReuseCurrentState=pool_boundary,
                  poolReuseCaptureArtifact='display05-pool-reuse-scan.json',
                  titleBeforeMaterial='material-display05-title-before.json',
                  poolReuseMaterial='material-display05-pool-reuse-boundary.json',
                  missingBoundaryClasses=([name for name, ok in [
                      ('production-scene-cut', scene_cut_ok),
                      ('pool-reuse-current-state-boundary', pool_reuse_ok)] if not ok])),
        check_row('DISPLAY-06', pause_status, False, machine_complete=pause_status == 'passed', pauseLabel='pause-menu', resumedLabel='pause-resumed',
                  worldFrozen=bool(pause and pause.get('worldFrozen')), frozenObjectCount=(pause or {}).get('counts', {}).get('frozen', 0),
                  pauseOwnerObjects=len(paused_objects), resumedWorldFrozen=(resumed or {}).get('worldFrozen')),
        check_row('DISPLAY-07', 'passed' if first_frame_ok else 'incomplete', False, machine_complete=False,
                  receipt=first_frame, evidenceBoundary='runtimeFinish observed successful SDL swap-count change before emitting first-frame',
                  browserCompositorSubmissionMeasured=False, physicalScanoutMeasured=False),
        check_row('DISPLAY-02', 'incomplete', False, machine_complete=False, reason='No physical 120/144 Hz display and compositor presentation trace was captured'),
        check_row('DISPLAY-08', 'incomplete', False, machine_complete=False, reason='No paired physical-device 60 Hz limiter settings were captured'),
    ]
    bundles = [
        {'id': 'continuous-motion-and-limit', 'adaptationIds': ['DISPLAY-02', 'DISPLAY-04', 'DISPLAY-08'],
         'materials': ['stage1-motion'], 'humanConclusion': 'existing visual-review record'},
        {'id': 'boundary-and-pause', 'adaptationIds': ['DISPLAY-04', 'DISPLAY-05', 'DISPLAY-06'],
         'materials': ['display05-title-before', 'display05-pool-reuse-boundary',
                       'stage1-motion', 'pause-menu', 'pause-resumed'],
         'humanConclusion': 'existing visual-review record'},
    ]
    return {'schema': 'eagler-presentation/display-observations/1',
            'scope': {'game': 'th08', 'variant': 'normal'}, 'checks': rows,
            'lifecycleDelta': delta, 'lifecycleBefore': lifecycle_before, 'lifecycleAfter': lifecycle_after,
            'poolReuseCapture': pool_reuse,
            'visualBundles': bundles,
            'humanReview': {'status': 'pending', 'groups': [bundle['id'] for bundle in bundles],
                            'selectedChecks': ['DISPLAY-02', 'DISPLAY-04', 'DISPLAY-05', 'DISPLAY-06', 'DISPLAY-08']},
            'claims': {'physicalRefreshVerified': False, 'browserCompositorSubmissionMeasured': False,
                       'humanVisualApproval': False}}


def missing_pair_report(fixture, reason, status='incomplete'):
    ids = ['DISPLAY-01', 'DISPLAY-02', 'DISPLAY-03', 'DISPLAY-07', 'REPLAY-07', 'AUDIO-06', 'VERIFY-07']
    return {'schema': 'eagler-presentation/replay-pair/1', 'scope': {'game': 'th08', 'variant': 'normal'},
            'status': status, 'issues': [reason], 'comparedTicks': 0, 'fixture': fixture,
            'checks': [check_row(item, status if item not in {'DISPLAY-02', 'DISPLAY-07'} else 'incomplete', False,
                                 machine_complete=False, reason=reason) for item in ids],
            'claims': {'controlledExtraDrawPurity': False, 'physicalRefreshVerified': False,
                       'replayNaturalTerminalVerified': False, 'rollbackAudioVerified': False}}


def combined_acceptance(pair, display):
    pair_rows = {item['id']: item for item in pair.get('checks', [])}
    display_rows = {item['id']: item for item in display.get('checks', [])}
    ids = ['DISPLAY-01', 'DISPLAY-02', 'DISPLAY-03', 'DISPLAY-04', 'DISPLAY-05', 'DISPLAY-06',
           'DISPLAY-07', 'DISPLAY-08', 'REPLAY-07', 'AUDIO-06', 'VERIFY-07']
    checks = []
    for adaptation_id in ids:
        rows = [row for row in (pair_rows.get(adaptation_id), display_rows.get(adaptation_id)) if row]
        if not rows:
            checks.append(check_row(adaptation_id, 'incomplete', False, machine_complete=False,
                                    reason='No paired or display observation was produced'))
            continue
        # The paired runs supply the machine boundary for first presentation and
        # repeated reads. The one-off display session adds context, but its
        # smaller sample cannot downgrade a valid paired receipt or complete it.
        paired_authoritative = adaptation_id in {'DISPLAY-07', 'VERIFY-07'}
        primary = pair_rows.get(adaptation_id) if paired_authoritative else None
        if primary is not None:
            status = primary.get('status') if primary.get('status') in {'passed', 'failed', 'incomplete'} else 'incomplete'
            complete = status == 'passed' and primary.get('complete') is True
            machine_complete = status == 'passed' and primary.get('machineComplete') is True
        elif paired_authoritative:
            status = 'failed' if any(row.get('status') == 'failed' for row in rows) else 'incomplete'
            complete = False
            machine_complete = False
        else:
            statuses = [row.get('status') for row in rows]
            status = 'failed' if 'failed' in statuses else 'passed' if all(value == 'passed' for value in statuses) else 'incomplete'
            complete = status == 'passed' and all(row.get('complete') is True for row in rows)
            machine_complete = all(row.get('machineComplete') is True for row in rows)
        evidence = {'sources': [{'schema': row.get('schema'), **row.get('evidence', {})} for row in rows]}
        if primary is not None:
            evidence['authoritativeSource'] = 'pairedReplayRuns'
        checks.append(check_row(adaptation_id, status, complete, machine_complete=machine_complete, **evidence))
    overall = 'failed' if any(row['status'] == 'failed' for row in checks) else \
              'passed' if all(row['status'] == 'passed' for row in checks) else 'incomplete'
    return {'schema': 'eagler-presentation/acceptance/1', 'scope': {'game': 'th08', 'variant': 'normal'},
            'status': overall, 'complete': overall == 'passed' and all(row['complete'] for row in checks),
            'checks': checks,
            'humanReview': display.get('humanReview', {'status': 'pending', 'groups': []}),
            'claims': {'physicalRefreshVerified': False, 'browserCompositorSubmissionMeasured': False,
                       'physicalScanoutMeasured': False, 'humanVisualApproval': False,
                       'replayNaturalTerminalVerified': False, 'rollbackAudioVerified': False}}


def export_review_material(page, path, *, require_motion=False):
    with page.expect_download() as download_info:
        page.locator('#frames').click()
    download_info.value.save_as(str(path))
    value = json.loads(path.read_text(encoding='utf-8'))
    assert len(value.get('images', [])) == 5
    assert all(frame.get('png', '').startswith('data:image/png;base64,') for frame in value['images'])
    if require_motion:
        assert len({frame['png'] for frame in value['images']}) > 1, 'Motion material contains no visible alpha change'
    return path.read_bytes()


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--url', default='http://127.0.0.1:8132/')
    parser.add_argument('--quick', action='store_true')
    parser.add_argument('--output', type=Path)
    parser.add_argument('--paired-replay', action='store_true',
                        help='Capture a named real Replay in fresh baseline and extra-draw Runtime epochs in this output directory.')
    parser.add_argument('--replay-fixture', choices=[f'demo{i}' for i in range(4)],
                        help='Named prepared Replay fixture for --paired-replay.')
    parser.add_argument('--paired-ticks', type=int, default=120,
                        help='Logical ticks per paired Replay run (1..3600; default 120).')
    parser.add_argument('--paired-cadences',
                        help='Production GameHost callback rates for --paired-replay, exactly 60,120,144.')
    parser.add_argument('--cadence-difference-groups',
                        help='The two measured non-authority cadence groups declared by the profile.')
    parser.add_argument('--expected-authority-groups', type=int,
                        help='Profile-declared authority group count for --paired-replay.')
    args = parser.parse_args()
    if args.paired_replay and args.quick:
        parser.error('--paired-replay requires the full visual-material capture; remove --quick')
    if args.paired_replay and not args.replay_fixture:
        parser.error('--paired-replay requires --replay-fixture=demo0..demo3')
    if args.replay_fixture and not args.paired_replay:
        parser.error('--replay-fixture requires --paired-replay')
    if not 1 <= args.paired_ticks <= 3600:
        parser.error('--paired-ticks must be between 1 and 3600')
    expected_cadences = '60,120,144'
    expected_difference_groups = 'host-frame-cadence,synchronous-audio-operations'
    if args.paired_replay and (not args.expected_authority_groups or args.expected_authority_groups < 1):
        parser.error('--paired-replay requires a positive --expected-authority-groups from the selected profile')
    if args.paired_replay and args.paired_cadences != expected_cadences:
        parser.error('--paired-replay requires --paired-cadences=60,120,144')
    if args.paired_replay and args.cadence_difference_groups != expected_difference_groups:
        parser.error('--paired-replay requires the profile-declared two cadence difference groups')
    if not args.paired_replay and (args.paired_cadences or args.cadence_difference_groups or args.expected_authority_groups):
        parser.error('Cadence and authority-count options require --paired-replay')
    OUT = args.output or EVIDENCE_ROOT / (datetime.now(timezone.utc).strftime('%Y%m%dT%H%M%SZ') + '-' + uuid4().hex[:8])
    OUT.mkdir(parents=True, exist_ok=False)
    errors = []
    reports = []
    visual_material_paths = []
    lifecycle_before = None
    lifecycle_after = None
    pool_reuse_capture = None
    with sync_playwright() as p:
        browser = p.chromium.launch(headless=True, args=['--enable-unsafe-swiftshader'])
        context = browser.new_context(viewport={'width': 1440, 'height': 1120}, service_workers='block')
        page = context.new_page()
        page.set_default_timeout(120000)
        page.on('pageerror', lambda error: errors.append(str(error)))
        try:
            page.goto(args.url, wait_until='load')
            page.wait_for_function('window.presentationLab !== undefined')
            assert page.locator('#reviewExport').is_disabled()
            page.locator('#boot').click()
            page.wait_for_function('!!window.presentationLab?.controller')
            page.wait_for_function('!document.querySelector("#inspect").disabled')
            for label, step in [('title-idle', 0), ('title-animate', 6)]:
                if step:
                    page.evaluate('n => presentationLab.controller.step(n)', step)
                r = page.evaluate(SAMPLE, {'label': label})
                reports.append(r)
                print(json.dumps(brief(r), ensure_ascii=False), flush=True)
            page.screenshot(path=str(OUT / 'title-lab.png'), full_page=True)
            if not args.quick:
                page.evaluate('''() => {const c=presentationLab.controller;
                  c.key('ArrowDown',true);c.step(3);c.key('ArrowDown',false);c.step(4);
                }''')
                for label, negative in [('menu-motion', False), ('negative-control', True), ('negative-recovered', False)]:
                    r = page.evaluate(SAMPLE, {'label': label, 'negativeControl': negative})
                    reports.append(r)
                    print(json.dumps(brief(r), ensure_ascii=False), flush=True)
                page.screenshot(path=str(OUT / 'menu-motion.png'), full_page=True)
                # Fresh iframe; navigate using the real game input owner.
                page.evaluate('presentationLab.boot().then(() => true)')
                title_before = page.evaluate(SAMPLE, {'label': 'display05-title-before'})
                reports.append(title_before)
                visual_material_paths.append(OUT / 'material-display05-title-before.json')
                export_review_material(page, visual_material_paths[-1])
                for i in range(5):
                    result = page.evaluate('''() => {const c=presentationLab.controller;c.press('KeyZ');
                      return {status:c.runtime.status(),trace:c.trace().slice(0,11),
                        screen:c.core.status(c.runtime.app,8),cursor:c.core.status(c.runtime.app,9)};
                    }''')
                    print(json.dumps({'navigate': i, **result}, ensure_ascii=False), flush=True)
                lifecycle_before = page.evaluate('''() => presentationLab.controller.observer.readLifecycleEvidence()''')
                pool_scan = page.evaluate('''() => {
                  const c=presentationLab.controller,start=c.observer.readLifecycleEvidence();
                  let capture=null,advancedTicks=0;
                  const candidate=snapshot=>{
                    if(!start.available||!snapshot.available||snapshot.dropped!==0||
                       snapshot.events.length<start.events.length||
                       JSON.stringify(snapshot.events.slice(0,start.events.length))!==JSON.stringify(start.events))return null;
                    const events=snapshot.events.slice(start.events.length);
                    for(let birthIndex=1;birthIndex<events.length;birthIndex++){
                      const born=events[birthIndex];if(born.operation!==1)continue;
                      for(let retireIndex=birthIndex-1;retireIndex>=0;retireIndex--){
                        const retired=events[retireIndex];
                        if(retired.operation===2&&retired.owner===born.owner&&retired.object===born.object&&
                           Number.isSafeInteger(retired.generation)&&Number.isSafeInteger(born.generation)&&
                           born.generation>retired.generation)
                          return {retired,born};
                      }
                    }
                    return null;
                  };
                  for(let tick=1;tick<=241;tick++){
                    c.step(1);
                    advancedTicks=tick;
                    const reuse=candidate(c.observer.readLifecycleEvidence());
                    if(reuse){
                      const report=c.sweep({label:'display05-pool-reuse-boundary'});
                      report.display05Boundary={event:reuse,source:'native lifecycle ring + same-tick Lab current state'};
                      presentationLab.renderReport(report);capture={event:reuse,report};break;
                    }
                  }
                  if(!capture){
                    const report=c.sweep({label:'display05-pool-reuse-boundary'});
                    report.display05Boundary={status:'incomplete',reason:'No retire then higher-generation birth for the same owner/object appeared in the 241-tick window'};
                    presentationLab.renderReport(report);capture={event:null,report};
                  }
                  return {capture,advancedTicks};
                }''')
                pool_reuse_capture = pool_scan.get('capture') if isinstance(pool_scan, dict) else None
                if isinstance(pool_reuse_capture, dict):
                    if isinstance(pool_reuse_capture.get('report'), dict):
                        reports.append(pool_reuse_capture['report'])
                    (OUT / 'display05-pool-reuse-scan.json').write_text(
                        json.dumps(pool_reuse_capture, ensure_ascii=False, indent=2), encoding='utf-8')
                visual_material_paths.append(OUT / 'material-display05-pool-reuse-boundary.json')
                export_review_material(page, visual_material_paths[-1])
                page.evaluate('remaining => {if(remaining>0)presentationLab.controller.step(remaining);}',
                               241 - pool_scan.get('advancedTicks', 0))
                page.evaluate('''() => {const c=presentationLab.controller;c.key('ArrowRight',true);c.step(4);c.key('ArrowRight',false);}''')
                for label, negative in [('stage1-motion', False), ('stage1-negative', True), ('stage1-recovered', False)]:
                    r = page.evaluate(SAMPLE, {'label': label, 'negativeControl': negative})
                    reports.append(r)
                    print(json.dumps(brief(r), ensure_ascii=False), flush=True)
                    if label == 'stage1-motion':
                        visual_material_paths.append(OUT / 'material-continuous-motion.json')
                        export_review_material(page, visual_material_paths[-1], require_motion=True)
                page.screenshot(path=str(OUT / 'stage1-motion.png'), full_page=True)
                lifecycle_after = page.evaluate('''() => presentationLab.controller.observer.readLifecycleEvidence()''')
                page.evaluate('presentationLab.controller.press("Escape", 4)')
                r = page.evaluate(SAMPLE, {'label': 'pause-menu'})
                reports.append(r)
                print(json.dumps(brief(r), ensure_ascii=False), flush=True)
                page.locator('#reviewGroup').check()
                page.locator('#reviewVerdict').select_option('inconclusive')
                page.locator('#reviewNotes').fill('Temporary fixture verdict before adding pause material.')
                visual_material_paths.append(OUT / 'material-pause-menu.json')
                export_review_material(page, visual_material_paths[-1])
                assert page.locator('#reviewVerdict').input_value() == '', 'Added pause material needs a new group verdict'
                page.screenshot(path=str(OUT / 'pause-menu.png'), full_page=True)
                page.evaluate('''() => {const c=presentationLab.controller;c.press('Escape',4);c.step(3);}''')
                resumed = page.evaluate(SAMPLE, {'label': 'pause-resumed'})
                reports.append(resumed)
                print(json.dumps(brief(resumed), ensure_ascii=False), flush=True)
                page.locator('#reviewVerdict').select_option('inconclusive')
                page.locator('#reviewNotes').fill('Temporary fixture verdict before adding resumed material.')
                visual_material_paths.append(OUT / 'material-pause-resumed.json')
                export_review_material(page, visual_material_paths[-1])
                assert page.locator('#reviewVerdict').input_value() == '', 'Added resumed material needs a new group verdict'
            (OUT / 'browser-captures.json').write_text(json.dumps({
                'build': page.evaluate('presentationLab.identity'),
                'browser': browser.version, 'lane': 'Playwright Chromium / SwiftShader / frozen alpha',
                'reports': reports, 'errors': errors,
            }, ensure_ascii=False, indent=2), encoding='utf-8')
            if errors:
                raise AssertionError(errors)
            assert all(r['valid'] for r in reports), 'Invalid capture window'
            normal_reports = [r for r in reports if not r.get('negativeControl')]
            negative_reports = [r for r in reports if r.get('negativeControl')]
            assert all(r['stateAuthority'].get('complete') and not r['stateAuthority'].get('samples') for r in reports), 'Native state fields are not completely mapped to shared authority domains'
            assert all(r['purityStatus'] == 'pass' for r in normal_reports), 'A normal presentation sweep changed an authoritative field or lacks complete coverage'
            if not args.quick:
                assert len(negative_reports) == 2
                for r in negative_reports:
                    assert r['purityStatus'] == 'fail', 'The real draw-side audio mutation escaped the purity assertion'
                    assert any(c['group'] == 'synchronous-audio-operations' for c in r['stateChanges']), 'Missing native audio side-effect receipt'
                by_label = {r['label']: r for r in reports}
                def player_position(label):
                    return next(f['status'] for o in by_label[label]['objects'] if o['ownerId'] == 13
                                for f in o['fields'] if f['id'] == 'position')
                assert player_position('stage1-motion') == 'interpolated'
                assert player_position('stage1-negative') == 'missing-interpolation'
                assert player_position('stage1-recovered') == 'interpolated'
                assert by_label['stage1-motion']['traceBefore'] == by_label['stage1-recovered']['traceAfter']
                assert by_label['pause-menu']['worldFrozen']
                assert by_label['pause-menu']['counts'].get('frozen', 0) > 0
                # The real inspector controls and retained report must work,
                # not just the Python-to-WASM path.
                page.locator('#rows tr').first.click()
                assert page.locator('#detail').inner_text().strip()
                exported_materials = [path.read_bytes() for path in visual_material_paths]
                motion_index = next(i for i, path in enumerate(visual_material_paths)
                                    if path.name == 'material-continuous-motion.json')
                frame_report = json.loads(exported_materials[motion_index].decode('utf-8'))
                assert len(frame_report['images']) == 5
                assert all(f['png'].startswith('data:image/png;base64,') for f in frame_report['images'])
                assert len({f['png'] for f in frame_report['images']}) > 1, 'Captured images are blank or duplicate'
                data_identity = frame_report['build']['inputs']['data']
                assert data_identity['bytes'] > 0 and len(data_identity['sha256']) == 64
                assert frame_report['build']['inputs']['replay'] is None
                assert frame_report['build']['toolSources']['eagler-common/review-record.mjs']
                assert page.locator('#reviewVerdict').input_value() == '', 'No default human verdict'
                assert page.locator('#reviewObservations').count() == 0, 'No per-check writing form'
                page.locator('#reviewer').fill('AUTOMATED UI FIXTURE')
                page.locator('#reviewVerdict').select_option('inconclusive')
                page.locator('#reviewNotes').fill('Automated export check. Pending an actual human visual review.')
                with page.expect_download() as review_info:
                    page.locator('#reviewExport').click()
                review_info.value.save_as(str(OUT / 'visual-review-fixture.json'))
                review = json.loads((OUT / 'visual-review-fixture.json').read_text(encoding='utf-8'))
                assert len(review['materials']) == len(exported_materials) == 5
                for material, raw in zip(review['materials'], exported_materials):
                    assert material['report']['bytes'] == len(raw)
                    assert material['report']['sha256'] == hashlib.sha256(raw).hexdigest()
                assert len(set(exported_materials)) == 5, 'Five distinct scene windows are required'
                assert review['verdict'] == 'inconclusive' and review['verdictScope'] == 'sampled-material-set'
                assert review['checks'] == [{'id': item, 'complete': False} for item in
                    ['DISPLAY-02', 'DISPLAY-04', 'DISPLAY-05', 'DISPLAY-06', 'DISPLAY-08']]
                assert review['claims']['checksComplete'] is False
                assert 'observations' not in review, 'No per-check written conclusions'
                checked = page.evaluate('''async ({record,materials}) => {
                  const {validateVisualReview}=await import('/common/testkit/presentation-lab/review-record.mjs');
                  return validateVisualReview({record,materials});
                }''', {'record': review, 'materials': [
                    {'filename': item['filename'], 'reportText': raw.decode('utf-8')}
                    for item, raw in zip(review['materials'], exported_materials)]})
                assert checked['valid']
                # UI identity fields persist for this review session.
                assert page.locator('#reviewer').input_value() == 'AUTOMATED UI FIXTURE'
                page.locator('#reviewGroup').uncheck()
                assert page.locator('#reviewVerdict').input_value() == '', 'Changed scope needs a new verdict'
                page.locator('#inspect').click()
                assert page.locator('#reviewVerdict').input_value() == ''
                with page.expect_download() as download_info:
                    page.locator('#export').click()
                download_info.value.save_as(str(OUT / 'exported-session.json'))
                page.locator('#import').set_input_files(str(OUT / 'exported-session.json'))
                page.wait_for_function('document.querySelector("#status").textContent.includes("离线报告模式")')
                assert page.locator('#play').is_disabled()
                page.locator('#import').set_input_files(str(visual_material_paths[0]))
                page.wait_for_function('!document.querySelector("#reportFrame").hidden && document.querySelector("#reportFrame").naturalWidth === 640')
                page.locator('#alpha').fill('0')
                page.wait_for_function('document.querySelector("#alphaValue").textContent.includes("0.00")')
                page.screenshot(path=str(OUT / 'report-inspection.png'), full_page=True)
                # Interactive use must work as well as deterministic stepping.
                page.evaluate('presentationLab.boot().then(() => true)')
                before_play = page.evaluate('presentationLab.controller.runtime.status()[9]')
                page.locator('#play').click()
                page.wait_for_function('before => presentationLab.controller.runtime.status()[9] > before + 5', arg=before_play)
                page.locator('#inspect').click()
                page.wait_for_function('presentationLab.report !== null')
                frozen_tick = page.evaluate('presentationLab.controller.runtime.status()[9]')
                page.wait_for_timeout(150)
                assert page.evaluate('presentationLab.controller.runtime.status()[9]') == frozen_tick
                first_frame = page.evaluate('''() => {
                  const c=presentationLab.controller, epoch=c.identity.runtimeEpoch;
                  const receipt=presentationLab.firstFrameReceipt(epoch)||{};
                  return {...receipt,runtimeEpoch:epoch,nativePresentCountAtReceipt:receipt.nativePresentCount,
                    expectedOrigin:location.origin};
                }''')
                display_observations = display_observation_report(
                    reports, lifecycle_before, lifecycle_after, first_frame, pool_reuse_capture)
                (OUT / 'presentation-observations.json').write_text(
                    json.dumps(display_observations, ensure_ascii=False, indent=2), encoding='utf-8')
                if len(display_observations['checks']) != 6:
                    raise AssertionError('Display observation checks are not registered as expected')
                if args.paired_replay:
                    baseline = controlled = cadence144 = None
                    pair_path = OUT / f'paired-replay-{args.replay_fixture}-result.json'
                    try:
                        fixture = page.evaluate('''async name => {
                          const response=await fetch(`/input/${name}.rpy`,{cache:'no-store'});
                          if(!response.ok)return {available:false,status:response.status};
                          const bytes=new Uint8Array(await response.arrayBuffer());
                          const sha256=Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',bytes)),
                            b=>b.toString(16).padStart(2,'0')).join('');
                          if(!bytes.length)return {available:false,status:200,reason:'empty Replay fixture'};
                          window.__presentationLabPairedReplay=Array.from(bytes);
                          return {available:true,bytes:bytes.byteLength,sha256};
                        }''', args.replay_fixture)
                        if not fixture.get('available'):
                            pair = missing_pair_report(args.replay_fixture,
                                f"Selected {args.replay_fixture}.rpy is unavailable from the prepared Lab input ({fixture})")
                        else:
                            baseline = page.evaluate('''async ({fixture,ticks,expectedAuthorityGroups}) => presentationLab.captureReplayRun({
                              replayBytes:window.__presentationLabPairedReplay,role:'baseline',ticks,fixture,cadenceHz:60,expectedAuthorityGroups})''',
                              {'fixture': args.replay_fixture, 'ticks': args.paired_ticks,
                               'expectedAuthorityGroups': args.expected_authority_groups})
                            (OUT / f'paired-replay-{args.replay_fixture}-baseline.json').write_text(
                                json.dumps(baseline, ensure_ascii=False, indent=2), encoding='utf-8')
                            controlled = page.evaluate('''async ({fixture,ticks,expectedAuthorityGroups}) => presentationLab.captureReplayRun({
                              replayBytes:window.__presentationLabPairedReplay,role:'controlled-extra-draw',ticks,
                              drawAlphas:[0,0.5,1],fixture,cadenceHz:120,expectedAuthorityGroups})''',
                              {'fixture': args.replay_fixture, 'ticks': args.paired_ticks,
                               'expectedAuthorityGroups': args.expected_authority_groups})
                            (OUT / f'paired-replay-{args.replay_fixture}-controlled-extra-draw.json').write_text(
                                json.dumps(controlled, ensure_ascii=False, indent=2), encoding='utf-8')
                            cadence144 = page.evaluate('''async ({fixture,ticks,expectedAuthorityGroups}) => presentationLab.captureReplayRun({
                              replayBytes:window.__presentationLabPairedReplay,role:'controlled-extra-draw',ticks,
                              drawAlphas:[0,0.5,1],fixture,cadenceHz:144,expectedAuthorityGroups})''',
                              {'fixture': args.replay_fixture, 'ticks': args.paired_ticks,
                               'expectedAuthorityGroups': args.expected_authority_groups})
                            (OUT / f'paired-replay-{args.replay_fixture}-cadence-144hz.json').write_text(
                                json.dumps(cadence144, ensure_ascii=False, indent=2), encoding='utf-8')
                            pair = page.evaluate('''({baseline,controlledExtraDraw,cadence144,requiredTicks,ratesHz,cadenceDifferenceGroups,expectedAuthorityGroups,fixture,input}) => {
                              const scope={game:'th08',variant:'normal'};
                              const cadenceComparison=presentationLab.compareReplayCadenceRuns({
                                runs:[baseline,controlledExtraDraw,cadence144],requiredTicks,ratesHz,scope,
                                expectedAuthorityGroups,cadenceDifferenceGroups});
                              const runList=[baseline,controlledExtraDraw,cadence144];
                              const drawEvidence=runList.slice(1).every(run=>run.status==='passed'&&run.samples.length===requiredTicks&&
                                run.samples.every(sample=>sample.drawReceipts.length===3&&sample.drawReceipts.every(receipt=>
                                  receipt.status==='drawn'&&receipt.unchanged===true&&receipt.before&&receipt.after)));
                              const display03=drawEvidence?'passed':runList.slice(1).some(run=>run.status==='failed')?'failed':'incomplete';
                              const firstFrameEvidence=runList.map(run=>run.firstFrame||null);
                              const display07=firstFrameEvidence.every(receipt=>receipt?.status==='passed'&&
                                Number.isSafeInteger(receipt.runtimeEpoch)&&receipt.runtimeEpoch===runList.find(run=>run.firstFrame===receipt)?.epoch&&
                                receipt.nativePresentCount>0)?'passed':'incomplete';
                              const production=baseline.identity?.build?.productionBuild;
                              const verify07=runList.every(run=>run.readVerification?.status==='passed'&&
                                run.readVerification.authorityGroups===expectedAuthorityGroups&&
                                run.readVerification.expectedAuthorityGroups===expectedAuthorityGroups&&
                                run.samples.length===requiredTicks)&&production?.available===true&&
                                production.sourceMatchesCurrent===true&&typeof production.wasmSha256==='string'&&
                                baseline.identity?.build?.diagnostic===true&&baseline.identity?.build?.instrumented===true
                                ?'passed':'incomplete';
                              const bound=(id,status,complete,evidence={})=>({id,status,complete,machineComplete:complete,
                                evidence:{game:scope.game,variant:scope.variant,adaptationIds:[id],...evidence}});
                              const checks=[...cadenceComparison.checks,
                                bound('DISPLAY-02','incomplete',false,{reason:'Physical display refresh and compositor timing are reviewed from device materials'}),
                                bound('DISPLAY-03',display03,display03==='passed',{extraDrawsPerLogicalTick:3,controlledCadences:[120,144]}),
                                bound('DISPLAY-07',display07,display07==='passed',{firstFrameReceipts:firstFrameEvidence,
                                  boundary:'same-epoch successful native SDL present count before Runtime first-frame event',
                                  compositorSubmissionMeasured:false,physicalScanoutMeasured:false}),
                                bound('VERIFY-07',verify07,verify07==='passed',{repeatedReadGroups:expectedAuthorityGroups,runtimeEpochs:runList.map(run=>run.epoch),
                                  productionBuild:production||null,diagnosticBuild:baseline.identity?.build||null})];
                              const issues=[...cadenceComparison.issues];
                              if(display03!=='passed')issues.push('controlled extra-draw purity did not pass at both high-rate callback schedules');
                              if(display07!=='passed')issues.push('same-epoch first-present receipt is missing');
                              if(verify07!=='passed')issues.push('production/diagnostic repeated-read evidence is incomplete');
                              const status=[cadenceComparison.status,display03,display07,verify07].includes('failed')?'failed':
                                cadenceComparison.status==='passed'&&display03==='passed'&&display07==='passed'&&verify07==='passed'?'passed':'incomplete';
                              return {schema:'eagler-presentation/replay-pair/1',scope,status,issues,fixture,input,
                                pairedTicks:requiredTicks,baselineComparison:null,cadenceComparison,checks,
                                runEvidence:runList.map(run=>({role:run.role,epoch:run.epoch,rateHz:run.scheduler?.rateHz,
                                  identity:run.identity,replayIdentity:run.replayIdentity,firstFrame:run.firstFrame})),
                                claims:{productionSchedulerCadenceVerified:cadenceComparison.status==='passed',
                                  controlledExtraDrawPurity:display03==='passed',physicalRefreshVerified:false,
                                  browserCompositorSubmissionMeasured:false,physicalScanoutMeasured:false,
                                  replayNaturalTerminalVerified:false,rollbackAudioVerified:false}};
                            }''',
                              {'baseline': baseline, 'controlledExtraDraw': controlled, 'cadence144': cadence144,
                               'requiredTicks': args.paired_ticks, 'ratesHz': [60, 120, 144],
                               'cadenceDifferenceGroups': expected_difference_groups.split(','),
                               'expectedAuthorityGroups': args.expected_authority_groups,
                               'fixture': args.replay_fixture, 'input': fixture})
                            pair['fixture'] = args.replay_fixture
                            pair['input'] = fixture
                    except Exception as error:
                        pair = missing_pair_report(args.replay_fixture,
                            f'Paired Replay capture failed: {type(error).__name__}: {error}', status='failed')
                    if len(errors) > 0:
                        pair.setdefault('issues', []).append('Browser page errors were recorded during the paired Lab session')
                        pair['status'] = 'failed'
                        for row in pair.get('checks', []):
                            if row.get('id') in {'DISPLAY-01', 'DISPLAY-03', 'REPLAY-07', 'AUDIO-06', 'VERIFY-07'}:
                                row['status'] = 'failed'
                                row['machineComplete'] = False
                    pair_path.write_text(json.dumps(pair, ensure_ascii=False, indent=2), encoding='utf-8')
                    acceptance = combined_acceptance(pair, display_observations)
                    (OUT / 'presentation-acceptance.json').write_text(
                        json.dumps(acceptance, ensure_ascii=False, indent=2), encoding='utf-8')
                    print(json.dumps({'pairedReplay': pair.get('status'), 'fixture': args.replay_fixture,
                                      'ticks': pair.get('comparedTicks', 0), 'issues': pair.get('issues', []),
                                      'humanReview': acceptance['humanReview']['status'],
                                      'acceptance': str(OUT / 'presentation-acceptance.json')},
                                     ensure_ascii=False), flush=True)
                    (OUT / 'browser-captures.json').write_text(json.dumps({
                        'build': page.evaluate('presentationLab.identity'),
                        'browser': browser.version, 'lane': 'Playwright Chromium / SwiftShader / frozen alpha',
                        'reports': reports, 'errors': errors,
                        'pairedReplay': {'fixture': args.replay_fixture, 'ticks': args.paired_ticks,
                                         'cadencesHz':[60,120,144], 'status': pair.get('status'), 'result': pair_path.name,
                                         'runs':[f'paired-replay-{args.replay_fixture}-baseline.json',
                                           f'paired-replay-{args.replay_fixture}-controlled-extra-draw.json',
                                           f'paired-replay-{args.replay_fixture}-cadence-144hz.json']},
                    }, ensure_ascii=False, indent=2), encoding='utf-8')
                    if pair.get('status') != 'passed':
                        raise AssertionError('Paired Replay observation did not pass: ' + '; '.join(pair.get('issues', [])))
            print(json.dumps({'browserCalibration': 'PASS', 'realWasm': True,
                              'negativeControl': not args.quick,
                              'stateAuthorityComplete': all(r['stateAuthority'].get('complete') and not r['stateAuthority'].get('samples') for r in reports),
                              'namedStatePurity': all(r['purityStatus'] == 'pass' for r in normal_reports),
                              'namedStatePurityStatus': sorted({r['purityStatus'] for r in normal_reports}),
                              'drawAudioNegativeRejected': bool(negative_reports) and all(r['purityStatus'] == 'fail' for r in negative_reports),
                              'reportExportImport': not args.quick, 'savedAlphaImages': not args.quick,
                              'humanReviewExportPlumbing': not args.quick, 'humanVisualApproval': False,
                              'livePlayAndFreeze': not args.quick}, ensure_ascii=False), flush=True)
        finally:
            if errors:
                print(json.dumps({'pageErrors': errors}, ensure_ascii=False), flush=True)
            page.screenshot(path=str(OUT / 'last-page.png'), full_page=True)
            browser.close()


if __name__ == '__main__':
    main()
