import fs from 'node:fs';
import path from 'node:path';
import { execFileSync } from 'node:child_process';
import { fileURLToPath } from 'node:url';

const scriptDir = path.dirname(fileURLToPath(import.meta.url));
const repoRoot = path.resolve(scriptDir, '../..');
const upstreamPath = path.resolve(repoRoot, '../thprac-reallyportable/thprac/src/thprac/thprac_th07.cpp');
const upstream = fs.readFileSync(upstreamPath, 'utf8');

// Pin the address-bearing decompilation that was used as the source-level
// reference for this port.  Do not silently follow a moving remote ref: if a
// newer decompilation should replace this one, review the map and update this
// hash deliberately.
const DECOMP_REF = '4c7b5b617969044cb5180403c07fc3ecfc4fadd3';

const sites = [];
for (const match of upstream.matchAll(/(?:EHOOK|PATCH)_(?:ST|DY)\((th07_[A-Za-z0-9_]+),\s*(0x[0-9A-Fa-f]+)/g)) {
    sites.push({ key: `${match[1]}@${Number.parseInt(match[2], 16).toString(16)}`, name: match[1], address: Number.parseInt(match[2], 16) });
}

let activeHotkey = null;
for (const line of upstream.split(/\r?\n/)) {
    const hotkey = line.match(/HOTKEY_DEFINE\(([^,]+),/);
    if (hotkey) activeHotkey = hotkey[1].trim();
    if (activeHotkey) {
        const patch = line.match(/(?:PATCH|EHOOK)_HK\((0x[0-9A-Fa-f]+)/);
        if (patch) {
            const address = Number.parseInt(patch[1], 16);
            sites.push({ key: `hotkey:${activeHotkey}@${address.toString(16)}`, name: `hotkey:${activeHotkey}`, address });
        }
    }
    if (line.includes('HOTKEY_ENDDEF')) activeHotkey = null;
}

const namedCount = sites.filter(site => !site.name.startsWith('hotkey:')).length;
const hotkeyCount = sites.length - namedCount;
if (namedCount !== 34 || hotkeyCount !== 11 || sites.length !== 45) {
    throw new Error(`TH07 thprac address inventory drifted: named=${namedCount}, hotkey=${hotkeyCount}, total=${sites.length}`);
}

const grep = execFileSync(
    'git',
    ['grep', '-n', 'FUNCTION: TH07', DECOMP_REF, '--', 'src/th07'],
    { cwd: repoRoot, encoding: 'utf8' },
);

const rawFunctions = [];
const functionLine = /^[^:]+:(src\/th07\/[^:]+):(\d+):.*FUNCTION: TH07 0x([0-9A-Fa-f]+)/;
for (const line of grep.split(/\r?\n/)) {
    const match = line.match(functionLine);
    if (match) {
        rawFunctions.push({
            address: Number.parseInt(match[3], 16),
            file: match[1],
            line: Number.parseInt(match[2], 10),
        });
    }
}
rawFunctions.sort((a, b) => a.address - b.address);

const sourceCache = new Map();
for (const fn of rawFunctions) {
    let lines = sourceCache.get(fn.file);
    if (!lines) {
        lines = execFileSync('git', ['show', `${DECOMP_REF}:${fn.file}`], { cwd: repoRoot, encoding: 'utf8' }).split(/\r?\n/);
        sourceCache.set(fn.file, lines);
    }
    fn.signature = '';
    for (const candidate of lines.slice(fn.line, fn.line + 8)) {
        const trimmed = candidate.trim();
        if (trimmed && !trimmed.startsWith('//')) {
            fn.signature = trimmed;
            break;
        }
    }
}

const expected = new Map([
    ['th07_all_clear_bonus_1@42b3d2', [0x42adab, 'src/th07/Gui.cpp', 'void Gui::UpdateGui()']],
    ['th07_all_clear_bonus_2@4280b7', [0x427f22, 'src/th07/Gui.cpp', 'u32 Gui::OnDraw(Gui *arg)']],
    ['th07_spell_bonus_display_fix1@42c80f', [0x42c577, 'src/th07/Gui.cpp', 'void Gui::DrawStageElements()']],
    ['th07_spell_bonus_display_fix2@42c889', [0x42c577, 'src/th07/Gui.cpp', 'void Gui::DrawStageElements()']],
    ['th07_spell_bonus_display_fix3@42c87c', [0x42c577, 'src/th07/Gui.cpp', 'void Gui::DrawStageElements()']],
    ['th07_rb@4157f3', [0x410520, 'src/th07/EclManager.cpp', 'ZunResult EclManager::RunEcl(Enemy *enemy)']],
    ['th07_enter@42eb08', [0x42e83e, 'src/th07/GameManager.cpp', 'ZunResult GameManager::AddedCallback(GameManager *arg)']],
    ['th07_border_break@441da4', [0x441bd0, 'src/th07/Player.cpp', 'void Player::BreakBorder(u32 unused)']],
    ['th07_reacquire_input@430f03', [0x430b50, 'src/th07/Controller.cpp', 'u16 Controller::GetInput()']],
    ['th07_soundplayer_queue_command@44d2f0', [0x44d2f0, 'src/th07/SoundPlayer.cpp', 'void SoundPlayer::PushCommand(AudioOpcode opcode, i32 arg1, const char *arg2)']],
    ['th07_prac_menu_1@45a214', [0x45a1dd, 'src/th07/MainMenu.cpp', 'u32 MainMenu::OnUpdateSelectPracticeStage()']],
    ['th07_prac_menu_3@45a65d', [0x45a1dd, 'src/th07/MainMenu.cpp', 'u32 MainMenu::OnUpdateSelectPracticeStage()']],
    ['th07_prac_menu_4@45a6d4', [0x45a1dd, 'src/th07/MainMenu.cpp', 'u32 MainMenu::OnUpdateSelectPracticeStage()']],
    ['th07_rep_menu_1@45ac43', [0x45a924, 'src/th07/MainMenu.cpp', 'u32 MainMenu::OnUpdateSelectReplay()']],
    ['th07_rep_menu_2@45af96', [0x45a924, 'src/th07/MainMenu.cpp', 'u32 MainMenu::OnUpdateSelectReplay()']],
    ['th07_rep_menu_3@45b2c1', [0x45a924, 'src/th07/MainMenu.cpp', 'u32 MainMenu::OnUpdateSelectReplay()']],
    ['th07_unpause_prevent_desync@403481', [0x402780, 'src/th07/AsciiManager.cpp', 'i32 PauseMenu::OnUpdate()']],
    ['th07_patch_main@42f2e3', [0x42e83e, 'src/th07/GameManager.cpp', 'ZunResult GameManager::AddedCallback(GameManager *arg)']],
    ['th07_disable_title@42956b', [0x428b19, 'src/th07/Gui.cpp', 'ZunResult Gui::ActualAddedCallback()']],
    ['th07_fake_shot@40e6ba', [0x40e5b0, 'src/th07/EclManager.cpp', 'i32 EclManager::GetVarValue(Enemy *enemy, i32 eclVar)']],
    ['th07_bgm@42f206', [0x42e83e, 'src/th07/GameManager.cpp', 'ZunResult GameManager::AddedCallback(GameManager *arg)']],
    ['th07_bgm_st6_1@427eda', [0x427e7c, 'src/th07/Gui.cpp', 'u32 Gui::OnUpdate(Gui *arg)']],
    ['th07_bgm_st6_2@42d9a5', [0x42d8d5, 'src/th07/GameManager.cpp', 'u32 GameManager::OnUpdate(GameManager *arg)']],
    ['th07_bgm_st6_3@40348a', [0x402780, 'src/th07/AsciiManager.cpp', 'i32 PauseMenu::OnUpdate()']],
    ['th07_save_replay@4443fd', [0x443da0, 'src/th07/ReplayManager.cpp', 'void ReplayManager::SaveReplay(const char *filename, char *replayName)']],
    ['th07_disable_prac_menu1@45b9ea', [0x45b9ad, 'src/th07/MainMenu.cpp', 'i32 MainMenu::DrawPracticeMenu()']],
    ['th07_disable_prac_menu2@45bb1c', [0x45b9ad, 'src/th07/MainMenu.cpp', 'i32 MainMenu::DrawPracticeMenu()']],
    ['th07_update@42fdf8', [0x42fd60, 'src/th07/Chain.cpp', 'i32 Chain::RunCalcChain()']],
    ['th07_render@42feb9', [0x42fe20, 'src/th07/Chain.cpp', 'i32 Chain::RunDrawChain()']],
    ['th07_disable_dataver@404fe0', [0x404fe0, 'src/th07/GameManager.hpp', 'i32 CheckGameIntegrity()']],
    ['th07_disable_demo@455a9a', [0x4555dd, 'src/th07/MainMenu.cpp', 'u32 MainMenu::OnUpdatePreInput()']],
    ['th07_disable_mutex@435bff', [0x435bd0, 'src/th07/GameWindow.cpp', 'ZunResult GameWindow::CheckForRunningGameInstance(HINSTANCE hInstance)']],
    ['th07_gui_init_1@45599d', [0x4555dd, 'src/th07/MainMenu.cpp', 'u32 MainMenu::OnUpdatePreInput()']],
    ['th07_gui_init_2@4351ac', [0x434bd0, 'src/th07/GameWindow.cpp', 'i32 GameWindow::InitD3dRendering()']],
    ['hotkey:mMuteki@43ee14', [0x43edc0, 'src/th07/Player.cpp', 'void Player::Die()']],
    ['hotkey:mInfLives@44116b', [0x440cf0, 'src/th07/Player.cpp', 'i32 Player::UpdateDeath()']],
    ['hotkey:mInfBombs@440bc7', [0x4409f0, 'src/th07/Player.cpp', 'void Player::UpdateBorderAndBombState()']],
    ['hotkey:mInfPower@440dd3', [0x440cf0, 'src/th07/Player.cpp', 'i32 Player::UpdateDeath()']],
    ['hotkey:mInfPower@440dbf', [0x440cf0, 'src/th07/Player.cpp', 'i32 Player::UpdateDeath()']],
    ['hotkey:mTimeLock@417726', [0x410520, 'src/th07/EclManager.cpp', 'ZunResult EclManager::RunEcl(Enemy *enemy)']],
    ['hotkey:mTimeLock@421f91', [0x420620, 'src/th07/EnemyManager.cpp', 'u32 EnemyManager::OnUpdate(EnemyManager *arg)']],
    ['hotkey:mTimeLock@4207a1', [0x420620, 'src/th07/EnemyManager.cpp', 'u32 EnemyManager::OnUpdate(EnemyManager *arg)']],
    ['hotkey:mAutoBomb@440d2c', [0x440cf0, 'src/th07/Player.cpp', 'i32 Player::UpdateDeath()']],
    ['hotkey:mAutoBomb@440d35', [0x440cf0, 'src/th07/Player.cpp', 'i32 Player::UpdateDeath()']],
    ['hotkey:mAutoBomb@440b8e', [0x4409f0, 'src/th07/Player.cpp', 'void Player::UpdateBorderAndBombState()']],
]);

if (expected.size !== 45) throw new Error(`TH07 expected address map drifted: ${expected.size}/45`);

const mappedFunctions = new Set();
for (const site of sites) {
    const expectation = expected.get(site.key);
    if (!expectation) throw new Error(`TH07 thprac address site has no decomp expectation: ${site.key}`);

    let owner = null;
    for (const fn of rawFunctions) {
        if (fn.address > site.address) break;
        owner = fn;
    }
    if (!owner) throw new Error(`TH07 thprac address has no preceding decomp function: ${site.key}`);

    const [expectedAddress, expectedFile, expectedSignature] = expectation;
    if (owner.address !== expectedAddress || owner.file !== expectedFile || owner.signature !== expectedSignature) {
        throw new Error(
            `${site.key}: decomp owner drifted; got ${owner.address.toString(16)} ${owner.file} ${owner.signature}; ` +
            `expected ${expectedAddress.toString(16)} ${expectedFile} ${expectedSignature}`,
        );
    }
    mappedFunctions.add(`${owner.file}:${owner.address.toString(16)}`);
}

console.log(
    `TH07 thprac address/decomp audit PASS: ${sites.length}/${sites.length} address-level sites mapped ` +
    `(${namedCount} named hooks + ${hotkeyCount} anonymous hotkey patch sites) across ${mappedFunctions.size} decompiled functions; ` +
    `decomp=${DECOMP_REF.slice(0, 12)}`,
);
