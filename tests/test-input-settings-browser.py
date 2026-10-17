"""Production React input settings and CSS in Chromium, with synthetic devices.

This verifies the editor, responsive layout and motion preferences. It does not
establish compatibility or latency for a physical controller or a complete game.
"""
import json
from pathlib import Path
import subprocess
import tempfile
from playwright.sync_api import sync_playwright

ROOT = Path(__file__).resolve().parents[1]
script = """import {build} from 'esbuild';
const result=await build({stdin:{resolveDir:process.cwd(),loader:'tsx',contents:`
import {createRoot} from 'react-dom/client';
import {LocaleProvider} from './app/i18n.tsx';
import {SettingsBody} from './app/components/settings/SettingsBody.tsx';
import {createGameSettingsModel} from './app/models/game-settings.ts';
import {createGamepadMappingModel} from './src/launcher/gamepad-mapping.mts';
import {createKeyboardPreferences} from './src/launcher/keyboard-preferences.mts';
import {createTh09LocalInput} from './src/launcher/th09-local-input.mts';
const saved=new Map(),storage={getItem:k=>saved.get(k)??null,setItem:(k,v)=>saved.set(k,v)};
window.pad={id:'Browser fixture controller',index:0,connected:true,mapping:'standard',buttons:Array.from({length:17},()=>({pressed:false,value:0})),axes:[0,0,0,0]};
window.gamepad=createGamepadMappingModel({storage,getGamepads:()=>[window.pad]});
window.keyboardPreferences=createKeyboardPreferences(storage);
window.th09Local=createTh09LocalInput(storage);
const model=createGameSettingsModel({storage});model.hydrate({productId:'th09',uiLocale:'zh-CN',languages:[{id:'ja',title:'日本語'}],musicAvailability:{audio:true,midiAvailable:true,remoteOggAdvertised:false},webMidiAvailable:false,mobile:false});
const midi={enabled:false,granted:false,busy:false,supported:false,outputs:[],selectedId:'',hint:''};
const actions={gamepad:window.gamepad,keyboard:window.keyboardPreferences,th09Local:window.th09Local,externalMidi:{subscribe:()=>()=>{},getSnapshot:()=>midi},file:async()=>{},confirm:async()=>true,showAppleNotice(){},reportError(e){throw e;}};
createRoot(document.getElementById('root')).render(<LocaleProvider locale="zh-CN"><SettingsBody model={model} actions={actions} onOpenTouchLayout={()=>{}}/></LocaleProvider>);
`},bundle:true,write:false,format:'iife',platform:'browser',jsx:'automatic',define:{'process.env.NODE_ENV':'"production"'}});
process.stdout.write(result.outputFiles[0].text);"""
bundle = subprocess.run(["node", "--input-type=module", "-e", script], cwd=ROOT,
                        capture_output=True, text=True, encoding="utf-8", check=True).stdout
shots = Path(tempfile.mkdtemp(prefix="eagler-input-settings-"))
with sync_playwright() as p:
    browser = p.chromium.launch(headless=True)
    for width in (800, 460, 360):
        page = browser.new_page(viewport={"width": width, "height": 1000})
        errors = []
        page.on("pageerror", lambda e: errors.append(str(e)))
        page.set_content('<div class="library-layout"><aside class="tools settings-ui" style="position:static;width:auto;max-width:none;height:auto;transform:none;opacity:1;padding:20px;background:#191a17;color:#eee9e1"><div id="root"></div></aside></div>')
        page.add_style_tag(path=str(ROOT / "public/styles.css"))
        page.add_style_tag(content=":root{--paper:#eee9e1;--muted:#a9a99c;--red-bright:#bd4438;--ui-font:Arial,sans-serif;--ease:cubic-bezier(.2,.8,.2,1)}body{margin:0;background:#121310}.tools{display:block!important;visibility:visible!important;pointer-events:auto!important}")
        page.add_script_tag(content=bundle)
        section = page.locator('.input-settings')
        section.wait_for()
        assert section.get_attribute('data-open') == 'true'
        assert page.evaluate("document.querySelector('.input-settings').compareDocumentPosition(document.querySelector('#advancedOptions')) & Node.DOCUMENT_POSITION_FOLLOWING")
        assert section.bounding_box()['height'] < 160
        section.get_by_role('button', name='键盘改键', exact=False).click()
        dialog = page.locator('.input-settings-dialog')
        assert dialog.bounding_box()['height'] <= 1000 * .89
        layout = page.locator('select[aria-label="键位布局"]')
        assert layout.input_value() == 'arrows'
        assert page.evaluate("Object.keys(keyboardPreferences.getSnapshot()).length") == 0
        assert page.locator('.keyboard-settings tr[data-action="up"] kbd').all_text_contents() == ['↑']
        layout.select_option('wasd', force=True)
        assert page.locator('.keyboard-settings tr[data-action="up"] kbd').all_text_contents() == ['W', '↑']
        row = page.locator('.keyboard-settings tr[data-action="shoot"]')
        row.locator('.input-binding-button').click()
        page.keyboard.press('q')
        assert row.locator('kbd').all_text_contents() == ['Q', 'Z']
        assert page.evaluate("keyboardPreferences.getSnapshot().KeyQ") == 'KeyZ'
        dialog.get_by_role('button', name='保存布局', exact=True).click()
        dialog.get_by_role('textbox', name='布局名称').fill('我的键位')
        dialog.get_by_role('button', name='保存', exact=True).click()
        saved_id = page.evaluate("keyboardPreferences.getLayoutSnapshot().selected")
        assert saved_id.startswith('saved-')
        layout.select_option('arrows', force=True)
        layout.select_option(saved_id, force=True)
        assert page.evaluate("keyboardPreferences.getSnapshot().KeyQ") == 'KeyZ'
        dialog.screenshot(path=str(shots / f'keyboard-layout-{width}.png'))
        dialog.get_by_role('button', name='关闭输入设置').click()
        dialog.wait_for(state='detached')
        heading = section.get_by_role('button', name='输入设置', exact=True)
        heading.click()
        assert page.evaluate("parseFloat(getComputedStyle(document.querySelector('.input-settings-collapse')).transitionDuration)>0")
        page.wait_for_timeout(280)
        assert section.locator('.input-settings-collapse').get_attribute('inert') is not None
        heading.click()
        page.wait_for_timeout(280)
        section.get_by_role('button', name='手柄改键', exact=False).click()
        dialog = page.locator('.input-settings-dialog')
        assert page.get_by_text('1P', exact=True).count() == 0
        assert page.get_by_text('2P', exact=True).count() == 0
        page.wait_for_function("gamepad.getSnapshot().devices.length===1")
        page.locator('select[aria-label="手柄设备"]').select_option(index=1, force=True)
        section.get_by_role('checkbox').check()
        shoot = page.locator('.gamepad-settings tr[data-action="shoot"]')
        shoot.locator('.input-binding-button').click()
        page.wait_for_timeout(80)
        page.evaluate("pad.buttons[3]={pressed:true,value:1}")
        page.wait_for_function("gamepad.getSnapshot().learning===null")
        assert shoot.locator('kbd').all_text_contents() == ['按钮 4']
        page.evaluate("pad.buttons[3]={pressed:false,value:0}")
        page.wait_for_timeout(100)
        assert page.evaluate("document.documentElement.scrollWidth<=innerWidth"), width
        shot = shots / f'input-settings-{width}.png'
        dialog.screenshot(path=str(shot))
        dialog.get_by_role('button', name='关闭输入设置').click()
        dialog.wait_for(state='detached')
        section.screenshot(path=str(shots / f'input-entries-{width}.png'))
        section.get_by_role('button', name='键盘改键', exact=False).click()
        page.locator('select[aria-label="配置场景"]').select_option('local', force=True)
        page.get_by_role('checkbox', name='启用本地双人配置').check()
        local_layout = page.locator('select[aria-label="双人键位布局"]')
        local_layout.select_option('wasd', force=True)
        page.get_by_role('tab', name='玩家 2', exact=True).click()
        local_layout.select_option('arrows', force=True)
        shoot = page.locator('.th09-local-input tr[data-action="shoot"]')
        shoot.locator('.input-binding-button').click()
        page.keyboard.press('w')
        assert page.get_by_text('W 已被另一位玩家使用', exact=True).is_visible()
        assert page.evaluate("th09Local.getSnapshot().players[1].bindings.shoot[0]") == 'Numpad1'
        page.keyboard.press('Escape')
        page.get_by_role('button', name='保存布局', exact=True).click()
        page.get_by_role('textbox', name='双人布局名称').fill('右手布局')
        page.get_by_role('button', name='保存', exact=True).click()
        local_id = page.evaluate("th09Local.getSnapshot().players[1].selected")
        local_layout.select_option('original', force=True)
        local_layout.select_option(local_id, force=True)
        assert page.evaluate("th09Local.getSnapshot().players[1].bindings.up[0]") == 'ArrowUp'
        page.locator('select[aria-label="玩家输入设备"]').select_option('gamepad', force=True)
        page.locator('select[aria-label="手柄设备"]').select_option(index=1, force=True)
        page.get_by_role('tab', name='玩家 1', exact=True).click()
        page.locator('select[aria-label="玩家输入设备"]').select_option('gamepad', force=True)
        page.locator('select[aria-label="手柄设备"]').select_option(index=1, force=True)
        assert page.get_by_text('这个手柄已分配给另一位玩家', exact=True).is_visible()
        assert page.evaluate("th09Local.getSnapshot().players[0].gamepad") == ''
        dialog.screenshot(path=str(shots / f'th09-local-{width}.png'))
        assert page.evaluate("document.documentElement.scrollWidth<=innerWidth")
        page.locator('select[aria-label="配置场景"]').select_option('general', force=True)
        page.emulate_media(reduced_motion='reduce')
        assert page.evaluate("getComputedStyle(document.querySelector('.input-device-panel')).animationName") == 'none'
        assert not errors, errors
        print(json.dumps({"width": width, "inputSettings": "PASS", "screenshot": str(shot)}), flush=True)
        page.close()
    browser.close()
