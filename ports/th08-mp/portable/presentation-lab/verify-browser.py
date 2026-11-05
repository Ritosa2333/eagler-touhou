"""Run real TH08 diagnostic captures in one isolated browser context.

Evidence goes to the ignored artifacts directory, never the launcher or retail saves.
The classifier unit suite is separate: these checks must actually boot the WASM.
"""
import argparse
import json
from pathlib import Path
from playwright.sync_api import sync_playwright

ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / 'artifacts/presentation-lab/evidence'

SAMPLE = """({label, negativeControl=false}) => {
  const c=presentationLab.controller, r=c.sweep({label,negativeControl});
  presentationLab.renderReport(r);
  return r;
}"""


def brief(report):
    return {k: report[k] for k in ('label', 'tick', 'valid', 'purityStatus', 'purity', 'gate', 'counts', 'coverage', 'stateChanges')}


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--url', default='http://127.0.0.1:8132/')
    parser.add_argument('--quick', action='store_true')
    args = parser.parse_args()
    OUT.mkdir(parents=True, exist_ok=True)
    errors = []
    reports = []
    with sync_playwright() as p:
        browser = p.chromium.launch(headless=True, args=['--enable-unsafe-swiftshader'])
        context = browser.new_context(viewport={'width': 1440, 'height': 1120}, service_workers='block')
        page = context.new_page()
        page.set_default_timeout(120000)
        page.on('pageerror', lambda error: errors.append(str(error)))
        try:
            page.goto(args.url, wait_until='load')
            page.wait_for_function('window.presentationLab !== undefined')
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
                for i in range(5):
                    result = page.evaluate('''() => {const c=presentationLab.controller;c.press('KeyZ');
                      return {status:c.runtime.status(),trace:c.trace().slice(0,11),
                        screen:c.core.status(c.runtime.app,8),cursor:c.core.status(c.runtime.app,9)};
                    }''')
                    print(json.dumps({'navigate': i, **result}, ensure_ascii=False), flush=True)
                page.evaluate('presentationLab.controller.step(241)')
                page.evaluate('''() => {const c=presentationLab.controller;c.key('ArrowRight',true);c.step(4);c.key('ArrowRight',false);}''')
                for label, negative in [('stage1-motion', False), ('stage1-negative', True), ('stage1-recovered', False)]:
                    r = page.evaluate(SAMPLE, {'label': label, 'negativeControl': negative})
                    reports.append(r)
                    print(json.dumps(brief(r), ensure_ascii=False), flush=True)
                page.screenshot(path=str(OUT / 'stage1-motion.png'), full_page=True)
                page.evaluate('presentationLab.controller.press("Escape", 4)')
                r = page.evaluate(SAMPLE, {'label': 'pause-menu'})
                reports.append(r)
                print(json.dumps(brief(r), ensure_ascii=False), flush=True)
                page.screenshot(path=str(OUT / 'pause-menu.png'), full_page=True)
            (OUT / 'browser-captures.json').write_text(json.dumps({
                'build': page.evaluate('presentationLab.identity'),
                'browser': browser.version, 'lane': 'Playwright Chromium / SwiftShader / frozen alpha',
                'reports': reports, 'errors': errors,
            }, ensure_ascii=False, indent=2), encoding='utf-8')
            if errors:
                raise AssertionError(errors)
            assert all(r['valid'] for r in reports), 'Invalid capture window'
            assert all(r['purityStatus'] != 'fail' for r in reports), 'A diagnostic sweep changed named authoritative fields'
            if not args.quick:
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
                with page.expect_download() as frames_info:
                    page.locator('#frames').click()
                frames_info.value.save_as(str(OUT / 'exported-frames.json'))
                frame_report = json.loads((OUT / 'exported-frames.json').read_text(encoding='utf-8'))
                assert len(frame_report['images']) == 5
                assert all(f['png'].startswith('data:image/png;base64,') for f in frame_report['images'])
                assert len({f['png'] for f in frame_report['images']}) > 1, 'Captured images are blank or duplicate'
                with page.expect_download() as download_info:
                    page.locator('#export').click()
                download_info.value.save_as(str(OUT / 'exported-session.json'))
                page.locator('#import').set_input_files(str(OUT / 'exported-session.json'))
                page.wait_for_function('document.querySelector("#status").textContent.includes("离线报告模式")')
                assert page.locator('#play').is_disabled()
                page.locator('#import').set_input_files(str(OUT / 'exported-frames.json'))
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
            print(json.dumps({'browserCalibration': 'PASS', 'realWasm': True,
                              'negativeControl': not args.quick,
                              'namedStatePurity': all(r['purityStatus'] == 'pass' for r in reports),
                              'namedStatePurityStatus': sorted({r['purityStatus'] for r in reports}),
                              'reportExportImport': not args.quick, 'savedAlphaImages': not args.quick,
                              'livePlayAndFreeze': not args.quick}, ensure_ascii=False), flush=True)
        finally:
            if errors:
                print(json.dumps({'pageErrors': errors}, ensure_ascii=False), flush=True)
            page.screenshot(path=str(OUT / 'last-page.png'), full_page=True)
            browser.close()


if __name__ == '__main__':
    main()
