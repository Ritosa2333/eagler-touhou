export const HOST_PROTOCOL = "eagler-touhou/1";
// Product features are declarations for genuine per-game differences. Required
// all-game adapter capabilities live in adapter-capabilities.mts and must not be
// added here as booleans that a title can opt out of.
//
// Keep the three publication layers explicit:
// - product: static capability ceiling/difference;
// - Runtime Release: concrete build attestation (all entries below);
// - Site Manifest: only features the Launcher must query as booleans at run
//   time. Some capabilities have a richer Host surface instead: languages are
//   represented by languages/languageOptions rather than a duplicate flag.
export const PRODUCT_FEATURE_POLICY = Object.freeze({
    thprac: Object.freeze({ runtimeRelease: true, hostManifestFeature: true, hostSurface: "features.thprac", hostValueSource: "server-request-and-runtime" }),
    languages: Object.freeze({ runtimeRelease: true, hostManifestFeature: false, hostSurface: "languages + languageOptions", hostValueSource: "language-catalog" }),
    focusHitbox: Object.freeze({ runtimeRelease: true, hostManifestFeature: true, hostSurface: "features.focusHitbox", hostValueSource: "runtime-release" }),
});
export const PRODUCT_FEATURE_IDS = Object.freeze(Object.keys(PRODUCT_FEATURE_POLICY));
export const HOST_RUNTIME_FEATURE_IDS = Object.freeze(Object.entries(PRODUCT_FEATURE_POLICY)
    .filter(([, policy]) => policy.hostManifestFeature)
    .map(([id]) => id));
export function isHostRuntimeFeatureId(value) {
    return HOST_RUNTIME_FEATURE_IDS.includes(value);
}
export function languagePriority(id) {
    return id === "ja" ? 0
        : id === "lang_zh-hans" ? 10
            : id === "lang_zh-hant" ? 11
                : id === "lang_en" ? 20
                    : 100;
}
const REIMU_A = Object.freeze({ labelKey: "multiplayer.loadout.reimuA", glyph: "霊", character: 0, shot: 0 });
const REIMU_B = Object.freeze({ labelKey: "multiplayer.loadout.reimuB", glyph: "霊", character: 0, shot: 1 });
const MARISA_A = Object.freeze({ labelKey: "multiplayer.loadout.marisaA", glyph: "魔", character: 1, shot: 0 });
const MARISA_B = Object.freeze({ labelKey: "multiplayer.loadout.marisaB", glyph: "魔", character: 1, shot: 1 });
const SAKUYA_A = Object.freeze({ labelKey: "multiplayer.loadout.sakuyaA", glyph: "咲", character: 2, shot: 0 });
const SAKUYA_B = Object.freeze({ labelKey: "multiplayer.loadout.sakuyaB", glyph: "咲", character: 2, shot: 1 });
const REIMU_C = Object.freeze({ labelKey: "multiplayer.loadout.reimuC", glyph: "霊", character: 0, shot: 2 });
const MARISA_C = Object.freeze({ labelKey: "multiplayer.loadout.marisaC", glyph: "魔", character: 1, shot: 2 });
const TH06_MULTIPLAYER_LOADOUTS = Object.freeze([REIMU_A, REIMU_B, MARISA_A, MARISA_B]);
const TH07_MULTIPLAYER_LOADOUTS = Object.freeze([...TH06_MULTIPLAYER_LOADOUTS, SAKUYA_A, SAKUYA_B]);
const TH08_MULTIPLAYER_LOADOUTS = Object.freeze([
    Object.freeze({ labelKey: "multiplayer.loadout.th08.reimuYukari", glyph: "霊", character: 0, shot: 0 }),
    Object.freeze({ labelKey: "multiplayer.loadout.th08.marisaAlice", glyph: "魔", character: 1, shot: 0 }),
    Object.freeze({ labelKey: "multiplayer.loadout.th08.sakuyaRemilia", glyph: "咲", character: 2, shot: 0 }),
    Object.freeze({ labelKey: "multiplayer.loadout.th08.youmuYuyuko", glyph: "妖", character: 3, shot: 0 }),
    Object.freeze({ labelKey: "multiplayer.loadout.th08.reimu", glyph: "霊", character: 4, shot: 0 }),
    Object.freeze({ labelKey: "multiplayer.loadout.th08.yukari", glyph: "紫", character: 5, shot: 0 }),
    Object.freeze({ labelKey: "multiplayer.loadout.th08.marisa", glyph: "魔", character: 6, shot: 0 }),
    Object.freeze({ labelKey: "multiplayer.loadout.th08.alice", glyph: "愛", character: 7, shot: 0 }),
    Object.freeze({ labelKey: "multiplayer.loadout.th08.sakuya", glyph: "咲", character: 8, shot: 0 }),
    Object.freeze({ labelKey: "multiplayer.loadout.th08.remilia", glyph: "蕾", character: 9, shot: 0 }),
    Object.freeze({ labelKey: "multiplayer.loadout.th08.youmu", glyph: "妖", character: 10, shot: 0 }),
    Object.freeze({ labelKey: "multiplayer.loadout.th08.yuyuko", glyph: "幽", character: 11, shot: 0 }),
]);
const TH10_MULTIPLAYER_LOADOUTS = Object.freeze([REIMU_A, REIMU_B, REIMU_C, MARISA_A, MARISA_B, MARISA_C]);
const TH11_MULTIPLAYER_LOADOUTS = Object.freeze([REIMU_A, REIMU_B, REIMU_C, MARISA_A, MARISA_B, MARISA_C]);
const TH12_MULTIPLAYER_LOADOUTS = Object.freeze([REIMU_A, REIMU_B, MARISA_A, MARISA_B,
    Object.freeze({ labelKey: "multiplayer.loadout.sanaeA", glyph: "早", character: 2, shot: 0 }),
    Object.freeze({ labelKey: "multiplayer.loadout.sanaeB", glyph: "早", character: 2, shot: 1 })]);
const TH13_MULTIPLAYER_LOADOUTS = Object.freeze(["reimu", "marisa", "sanae", "youmu"].map((name, character) => Object.freeze({ labelKey: `multiplayer.loadout.th13_${name}`, glyph: ["霊", "魔", "早", "妖"][character], character, shot: 0 })));
const TH15_MULTIPLAYER_LOADOUTS = Object.freeze(["reimu", "marisa", "sanae", "reisen"].map((name, character) => Object.freeze({ labelKey: `multiplayer.loadout.th15_${name}`, glyph: ["霊", "魔", "早", "鈴"][character], character, shot: 0 })));
const TH17_MULTIPLAYER_LOADOUTS = Object.freeze(["reimu","marisa","youmu"].flatMap((name,character)=>
    ["wolf","otter","eagle"].map((animal,shot)=>Object.freeze({
        labelKey:`multiplayer.loadout.th17_${name}_${animal}`, glyph:["霊","魔","妖"][character],character,shot,
        selection:Object.freeze([
            Object.freeze({value:character,labelKey:`multiplayer.character.th17_${name}`}),
            Object.freeze({value:shot,labelKey:`multiplayer.spirit.th17_${animal}`}),
        ]),
    }))));
const STANDARD_MULTIPLAYER_DIFFICULTIES = Object.freeze(["Easy", "Normal", "Hard", "Lunatic", "Extra"]);
const TH07_MULTIPLAYER_DIFFICULTIES = Object.freeze([...STANDARD_MULTIPLAYER_DIFFICULTIES, "Phantasm"]);
const TH09_MULTIPLAYER_DIFFICULTIES = Object.freeze(["Easy", "Normal", "Hard", "Lunatic"]);
const TH19_MULTIPLAYER_DIFFICULTIES = Object.freeze(["Easy", "Normal", "Hard", "Lunatic"]);
// Character IDs follow TH09's original character_labels table in TitleData.inc.
const TH09_MULTIPLAYER_GLYPHS = Object.freeze(["霊", "魔", "咲", "妖", "鈴", "チ", "リ", "ミ", "て", "幽", "文", "メ", "小", "映", "メ", "ル"]);
const TH09_MULTIPLAYER_LOADOUTS = Object.freeze(Array.from({ length: 16 }, (_, character) => Object.freeze({ labelKey: `multiplayer.loadout.th09_${character}`, glyph: TH09_MULTIPLAYER_GLYPHS[character], character, shot: 0 })));
const TH075_MULTIPLAYER_GLYPHS = Object.freeze(["霊","魔","咲","愛","パ","妖","蕾","幽","紫","萃","美"]);
const TH075_MULTIPLAYER_LOADOUTS = Object.freeze(TH075_MULTIPLAYER_GLYPHS.flatMap((glyph, character) =>
    Array.from({length:9}, (_,shot) => Object.freeze({labelKey:`multiplayer.loadout.th075_${character}_${shot}`,glyph,character,shot,
        selection:Object.freeze([
            Object.freeze({value:character,labelKey:`multiplayer.character.th075_${character}`}),
            Object.freeze({value:shot%3,labelKey:`multiplayer.spell.th075_${character}_${shot%3}`}),
            Object.freeze({value:Math.floor(shot/3),labelKey:`multiplayer.spell.th075_${character}_${3+Math.floor(shot/3)}`}),
        ]),
    }))));
const STANDARD_MULTIPLAYER_PLAYER_COUNTS = Object.freeze([2, 3]);
const TOGGLE_TOUCH_FIRE = Object.freeze({ mode: "toggle", labelKey: "touch.tapToggle" });
const CHARGE_TOUCH_FIRE = Object.freeze({
    mode: "held-key",
    labelKey: "touch.holdFireCharge",
    key: Object.freeze({ code: "KeyZ", key: "z", keyCode: 90 }),
});
const TH19_CHARACTERS=Object.freeze(["霊夢", "魔理沙", "早苗", "藍", "あうん", "ナズーリン", "清蘭", "燐", "典", "マミゾウ", "八千慧", "早鬼", "尤魔", "萃香", "美天", "慧ノ子", "ちやり", "日狭美", "残無"]);
const TH01_REIMU = Object.freeze({ labelKey: 'multiplayer.loadout.th01_reimu', glyph: '霊', character: 0, shot: 0 });
const TH03_TOUCH_FIRE = Object.freeze({
    mode: "held-key",
    labelKey: "touch.actionHold",
    key: Object.freeze({ code: "KeyZ", key: "z", keyCode: 90 }),
});
export const PRODUCT_GAMES = Object.freeze({
    th01: Object.freeze({
        testOnly: true, buildOptIn: true, number: '01', title: '東方靈異伝', subtitle: 'Highly Responsive to Prayers', cardArtwork: 'th01-card.webp',
        storage: Object.freeze({saveRoot:'/savesth01',scoreFile:'scores.dat',configFiles:Object.freeze([])}),
        runtime:'./runtime/th01/th01.html', runtimeFileLayout:'directory', runtimeFileTree:true,
        runtimeAssets:Object.freeze(['th01.html','manifest.json','managed-shell.mjs','managed-data.mjs','managed-worker.mjs']), requiredShared:Object.freeze([]),
        dataProvider:'retail-memory', runtimeResourceTransport:'resources',
        package:Object.freeze({dataFileId:'game-data',dataTarget:'/th01.data',musicSourceDirectories:Object.freeze({ogg:'music'}),musicMounts:Object.freeze({ogg:'/bgm/akyu'})}),
        musicCapabilities:Object.freeze({midi:false}),musicRuntime:Object.freeze({localOggConfigureMode:'runtime-selection',embeddedOgg:true,sources:Object.freeze(['original','akyu'])}),touchFire:TH03_TOUCH_FIRE,
        support:Object.freeze({highRefreshRate:false,sourceRepository:'https://github.com/IxinorTyan/th01-multiorb-lab',adaptationNotice:'early-test'}),
        replay:Object.freeze({prefix:'th01',extensions:Object.freeze(['th01r']),naming:'filename'}),
        multiplayerRuntime:'./runtime/th01/multiplayer/th01.html',
        multiplayer:Object.freeze({titleKey:'game.title.th01mp',gameplay:'cooperative',playerCounts:STANDARD_MULTIPLAYER_PLAYER_COUNTS,difficulties:TH09_MULTIPLAYER_DIFFICULTIES,
            loadouts:Object.freeze([TH01_REIMU]),peerTransportGlobal:'__th01PeerTransport',relaySourceEnvelope:true,spectator:true,titleRoomEntry:false,preflightWithoutRoom:true,challengeMode:false,
            inputTiming:Object.freeze({rollbackLimit:8,sendPredictionLimit:8,rollbackDefault:true,runtimeTiming:true})}),
        features:Object.freeze({thprac:false,languages:false,focusHitbox:false}),
    }),
    th02: Object.freeze({
        practiceUI: 'browser',
        testOnly: true, buildOptIn: true, number: '02', title: '東方封魔録', subtitle: 'the Story of Eastern Wonderland', cardArtwork: 'th02-card.webp',
        storage: Object.freeze({saveRoot:'/savesth02',scoreFile:'HUUHI.DAT',configFiles:Object.freeze(["HUUMA.CFG"])}),
        runtime:'./runtime/th02/th02.html', runtimeFileLayout:'directory', runtimeFileTree:true,
        runtimeAssets:Object.freeze(['th02.html','manifest.json','managed-shell.mjs','managed-data.mjs','managed-worker.mjs']), requiredShared:Object.freeze([]),
        dataProvider:'retail-memory', runtimeResourceTransport:'resources',
        package:Object.freeze({dataFileId:'game-data',dataTarget:'/th02.data',musicSourceDirectories:Object.freeze({ogg:'music'}),musicMounts:Object.freeze({ogg:'/bgm/akyu'})}),
        musicCapabilities:Object.freeze({midi:false}),musicRuntime:Object.freeze({localOggConfigureMode:'runtime-selection',embeddedOgg:true,sources:Object.freeze(['original','akyu'])}),touchFire:TOGGLE_TOUCH_FIRE,
        support:Object.freeze({highRefreshRate:false,sourceRepository:'https://github.com/IxinorTyan/th02-coop-lab',adaptationNotice:'early-test'}),
        replay:Object.freeze({prefix:'th02',extensions:Object.freeze(['th02r']),naming:'filename'}),
        multiplayerRuntime:'./runtime/th02/multiplayer/th02.html',
        multiplayer:Object.freeze({titleKey:'game.title.th02mp',gameplay:'cooperative',playerCounts:STANDARD_MULTIPLAYER_PLAYER_COUNTS,difficulties:STANDARD_MULTIPLAYER_DIFFICULTIES,
            loadouts:Object.freeze(['fast','balanced','slow'].map((name,shot)=>Object.freeze({labelKey:'multiplayer.loadout.th02_'+name,glyph:'霊',character:0,shot}))),peerTransportGlobal:'__th02PeerTransport',relaySourceEnvelope:true,spectator:true,titleRoomEntry:false,preflightWithoutRoom:true,challengeMode:true,
            inputTiming:Object.freeze({rollbackLimit:12,sendPredictionLimit:6,rollbackDefault:true,runtimeTiming:true})}),
        features:Object.freeze({thprac:false,languages:false,focusHitbox:false}),
    }),
    th03: Object.freeze({
        testOnly: true, buildOptIn: true, number: '03', title: '東方夢時空', subtitle: 'Phantasmagoria of Dim.Dream', cardArtwork: 'th03-card.webp',
        storage: Object.freeze({saveRoot:'/savesth03',scoreFile:'YUME.NEM',configFiles:Object.freeze(["YUME.CFG"])}),
        runtime:'./runtime/th03/th03.html', runtimeFileLayout:'directory', runtimeFileTree:true,
        runtimeAssets:Object.freeze(['th03.html','manifest.json','managed-shell.mjs','managed-data.mjs','managed-worker.mjs']), requiredShared:Object.freeze([]),
        dataProvider:'retail-memory', runtimeResourceTransport:'resources',
        package:Object.freeze({dataFileId:'game-data',dataTarget:'/th03.data',musicSourceDirectories:Object.freeze({ogg:'music'}),musicMounts:Object.freeze({ogg:'/bgm/akyu'})}),
        musicCapabilities:Object.freeze({midi:false}),musicRuntime:Object.freeze({localOggConfigureMode:'runtime-selection',embeddedOgg:true,sources:Object.freeze(['original','akyu'])}),touchFire:TOGGLE_TOUCH_FIRE,
        support:Object.freeze({highRefreshRate:false,sourceRepository:'https://github.com/IxinorTyan/th03-netplay-lab',adaptationNotice:'early-test'}),
        replay:Object.freeze({prefix:'th03',extensions:Object.freeze(['th03r']),naming:'filename'}),
        multiplayerRuntime:'./runtime/th03/multiplayer/th03.html',
        multiplayer:Object.freeze({titleKey:'game.title.th03mp',gameplay:'versus',inGameSelection:true,playerCounts:Object.freeze([2]),difficulties:TH09_MULTIPLAYER_DIFFICULTIES,
            loadouts:Object.freeze([REIMU_A]),peerTransportGlobal:'__th03PeerTransport',relaySourceEnvelope:true,spectator:true,titleRoomEntry:false,preflightWithoutRoom:true,challengeMode:false,
            inputTiming:Object.freeze({rollbackLimit:12,sendPredictionLimit:6,rollbackDefault:true,runtimeTiming:true})}),
        features:Object.freeze({thprac:false,languages:false,focusHitbox:false}),
    }),
    // support.highRefreshRate describes currently implemented presentation for
    // Launcher UI. False keeps unfinished adapters honest; it does not waive
    // the required presentation-cadence acceptance gate.
    th04: Object.freeze({
        practiceUI: 'browser',
        testOnly: true, buildOptIn: true, number: '04', title: '東方幻想郷', subtitle: 'Lotus Land Story', cardArtwork: 'th04-card.webp',
        storage: Object.freeze({saveRoot:'/savesth04',scoreFile:'GENSOU.SCR',configFiles:Object.freeze(['MIKO.CFG'])}),
        runtime:'./runtime/th04/th04.html', runtimeFileLayout:'directory', runtimeFileTree:true,
        runtimeAssets:Object.freeze(['th04.html','manifest.json','managed-shell.mjs','managed-data.mjs','managed-worker.mjs']), requiredShared:Object.freeze([]),
        dataProvider:'retail-memory', runtimeResourceTransport:'resources',
        package:Object.freeze({dataFileId:'game-data',dataTarget:'/th04.data',musicSourceDirectories:Object.freeze({ogg:'music'}),musicMounts:Object.freeze({ogg:'/bgm/akyu'})}),
        musicCapabilities:Object.freeze({midi:false}),musicRuntime:Object.freeze({localOggConfigureMode:'runtime-selection',embeddedOgg:true,sources:Object.freeze(['original','akyu'])}),touchFire:TOGGLE_TOUCH_FIRE,
        support:Object.freeze({highRefreshRate:false,sourceRepository:'https://github.com/IxinorTyan/th04-coop-lab',adaptationNotice:'early-test'}),
        replay:Object.freeze({prefix:'th04',extensions:Object.freeze(['th04r']),naming:'filename'}),
        multiplayerRuntime:'./runtime/th04/multiplayer/th04.html',
        multiplayer:Object.freeze({titleKey:'game.title.th04mp',gameplay:'cooperative',playerCounts:STANDARD_MULTIPLAYER_PLAYER_COUNTS,difficulties:STANDARD_MULTIPLAYER_DIFFICULTIES,
            loadouts:TH06_MULTIPLAYER_LOADOUTS,peerTransportGlobal:'__th04PeerTransport',relaySourceEnvelope:true,spectator:true,titleRoomEntry:false,preflightWithoutRoom:true,challengeMode:true,
            inputTiming:Object.freeze({rollbackLimit:12,sendPredictionLimit:6,rollbackDefault:true,runtimeTiming:true})}),
        features:Object.freeze({thprac:false,languages:false,focusHitbox:false}),
    }),
    // support.highRefreshRate describes currently implemented presentation for
    // Launcher UI. False keeps unfinished adapters honest; it does not waive
    // the required presentation-cadence acceptance gate.
    th05: Object.freeze({
        testOnly: false, number: "05", title: "東方怪綺談", subtitle: "Mystic Square", cardArtwork: "th05-card.webp",
        storage: Object.freeze({saveRoot: "/savesth05", scoreFile: "score.dat", configFiles: Object.freeze(["game.cfg", "thprac-uth05.ini"])}),
        multiplayerRuntime: "./runtime/th05/multiplayer/th05.html",
        multiplayer: Object.freeze({
            titleKey: "game.title.th05mp", gameplay: "cooperative",
            playerCounts: STANDARD_MULTIPLAYER_PLAYER_COUNTS, difficulties: STANDARD_MULTIPLAYER_DIFFICULTIES,
            loadouts: Object.freeze(["reimu","marisa","mima","yuuka"].map((name,character)=>Object.freeze({labelKey:`multiplayer.loadout.th05_${name}`,glyph:["霊","魔","魅","幽"][character],character,shot:0}))),
            peerTransportGlobal: "__th05PeerTransport", relaySourceEnvelope: true, spectator: true, titleRoomEntry: false,
            inputTiming: Object.freeze({rollbackLimit:8,sendPredictionLimit:8,rollbackDefault:false,measuredStartup:true,manualDelayLimit:9}),
        }),
        runtime: "./runtime/th05/th05.html", runtimeFileLayout: "directory", dataProvider: "retail-memory", runtimeResourceTransport: "resources",
        requiredShared: Object.freeze(["/msgothic.ttc"]),
        runtimeAssets: Object.freeze(["th05.html", "th05.mjs", "th05.wasm", "shell.mjs", "managed.mjs", "data.mjs", "storage.mjs", "input.mjs", "pointer.mjs", "audio.mjs", "audio-layout.mjs", "music-notes.mjs", "sfx.mjs", "playback.mjs", "replay.mjs", "clock.mjs", "directory-keyboard.mjs", "keyboard-owners.mjs", "keyboard-input-bridge.mjs", "direct-input-bridge.mjs", "manifest.json"]),
        musicCapabilities: Object.freeze({midi: false}), musicRuntime: Object.freeze({localOggConfigureMode: "runtime-selection", sources: Object.freeze(["original", "akyu"])}),
        thpracRuntime: Object.freeze({session: "native"}),
        touchFire: TOGGLE_TOUCH_FIRE,
        support: Object.freeze({highRefreshRate: false, sourceRepository: "https://github.com/Goan114/uth05win-vibe", adaptationNotice: "early-test"}),
        package: Object.freeze({dataFileId: "game-data", dataTarget: "/th05.data", rawDataImport: Object.freeze({fileNames: Object.freeze(["th05.data"])}),
            musicSourceDirectories: Object.freeze({ogg: "music"}), musicMounts: Object.freeze({ogg: "/music"})}),
        replay: Object.freeze({prefix: "th05", slotPrefix: "replay", slots: 100, extensions: Object.freeze(["rpy"])}), features: Object.freeze({thprac: true, languages: true, focusHitbox: true}),
    }),
    // support.highRefreshRate describes currently implemented presentation for
    // Launcher UI. False keeps unfinished adapters honest; it does not waive
    // the required presentation-cadence acceptance gate.
    hsob: Object.freeze({
        testOnly: true,
        importOnly: true,
        runtimeResourceTransport: "resources",
        cardArtwork: "hsob-card.webp",
        number: "HSOB",
        title: "東方真珠島",
        subtitle: "Hollow Song of Birds",
        storage: Object.freeze({
            saveRoot: "/savehsob",
            scoreFile: "Package_common_SaveData.dat",
            configFiles: Object.freeze(["config.dat", "Package_common_Config.dat"]),
        }),
        runtime: "./runtime/hsob/hsob.html",
        musicCapabilities: Object.freeze({ midi: false }),
        musicRuntime: Object.freeze({ localOggConfigureMode: "runtime-selection" }),
        touchFire: TOGGLE_TOUCH_FIRE,
        support: Object.freeze({
            highRefreshRate: false,
            sourceRepository: "https://github.com/Ritosa2333/ph3-modern",
            adaptationNotice: "early-test",
        }),
        runtimeFileLayout: "directory",
        requiredShared: Object.freeze(["/msgothic.ttc"]),
        runtimeAssets: Object.freeze([
            "hsob.html",
            "shell.mjs",
            "eagler-host.mjs",
            "manifest.json",
            "ph3-open.mjs",
            "ph3-open.wasm",
            "build.json",
        ]),
        dataProvider: "retail-memory",
        package: Object.freeze({
            dataFileId: "game-data",
            dataTarget: "/hsob.data",
            musicSourceDirectories: Object.freeze({ ogg: "music" }),
            musicMounts: Object.freeze({ ogg: "/game/script/th_hsob/music" }),
        }),
        replay: Object.freeze({ prefix: "Main_replay", extensions: Object.freeze(["dat"]) }),
        features: Object.freeze({ thprac: false, languages: false, focusHitbox: false }),
    }),
    th06: Object.freeze({
        cardArtwork: "th06-card.webp",
        number: "06",
        title: "東方紅魔郷",
        subtitle: "the Embodiment of Scarlet Devil",
        storage: Object.freeze({
            saveRoot: "/savesth06",
            scoreFile: "score.dat",
            configFiles: Object.freeze(["東方紅魔郷.cfg", "th06.cfg"]),
        }),
        runtime: "./runtime/th06/th06.html",
        musicCapabilities: Object.freeze({ midi: true }),
        musicRuntime: Object.freeze({ localOggConfigureMode: "midi-sentinel" }),
        touchFire: TOGGLE_TOUCH_FIRE,
        support: Object.freeze({ highRefreshRate: true,
            sourceRepository: "https://github.com/YomotsuHisami/th06",
        }),
        dataProvider: "emscripten-preload",
        package: Object.freeze({
            dataFileId: "game-data",
            dataTarget: "/th06.data",
            musicSourceDirectories: Object.freeze({ wav: "bgm", ogg: "bgm" }),
            musicMounts: Object.freeze({ wav: "/bgm", ogg: "/bgm" }),
        }),
        replay: Object.freeze({ prefix: "th6" }),
        multiplayerRuntime: "./runtime/th06/multiplayer/th06.html",
        multiplayer: Object.freeze({
            titleKey: "game.title.th06mp",
            gameplay: "cooperative",
            playerCounts: STANDARD_MULTIPLAYER_PLAYER_COUNTS,
            difficulties: STANDARD_MULTIPLAYER_DIFFICULTIES,
            loadouts: TH06_MULTIPLAYER_LOADOUTS,
            peerTransportGlobal: "__th06PeerTransport",
            spectator: true,
            titleRoomEntry: false,
        }),
        features: Object.freeze({ thprac: true, languages: true, focusHitbox: true }),
    }),
    th07: Object.freeze({
        cardArtwork: "th07-card.webp",
        cardPresentation: Object.freeze({
            positionPercent: 58,
            artBrightness: 0.42,
            artSaturation: 0.62,
            glowBrightness: 0.8,
            glowSaturation: 0.88,
        }),
        number: "07",
        title: "東方妖々夢",
        subtitle: "Perfect Cherry Blossom",
        storage: Object.freeze({
            saveRoot: "/savesth07",
            scoreFile: "score.dat",
            configFiles: Object.freeze(["th07.cfg"]),
        }),
        runtime: "./runtime/th07/th07.html",
        musicCapabilities: Object.freeze({ midi: true }),
        musicRuntime: Object.freeze({ localOggConfigureMode: "midi-sentinel" }),
        touchFire: TOGGLE_TOUCH_FIRE,
        support: Object.freeze({ highRefreshRate: true,
            sourceRepository: "https://github.com/YomotsuHisami/th07",
        }),
        dataProvider: "emscripten-preload",
        package: Object.freeze({
            dataFileId: "game-data",
            dataTarget: "/th07.data",
            musicSourceDirectories: Object.freeze({ wav: ".", ogg: "bgm-ogg" }),
            musicMounts: Object.freeze({ wav: "/", ogg: "/bgm-ogg" }),
        }),
        replay: Object.freeze({ prefix: "th7" }),
        multiplayerRuntime: "./runtime/th07/multiplayer/th07.html",
        multiplayer: Object.freeze({
            titleKey: "game.title.th07mp",
            gameplay: "cooperative",
            playerCounts: STANDARD_MULTIPLAYER_PLAYER_COUNTS,
            difficulties: TH07_MULTIPLAYER_DIFFICULTIES,
            loadouts: TH07_MULTIPLAYER_LOADOUTS,
            peerTransportGlobal: "__th07PeerTransport",
            spectator: true,
            titleRoomEntry: false,
        }),
        features: Object.freeze({ thprac: true, languages: true, focusHitbox: false }),
    }),
    th075: Object.freeze({
        testOnly: true,
        number: "07.5", title: "東方萃夢想", subtitle: "Immaterial and Missing Power", cardArtwork: "th075-card.webp",
        storage: Object.freeze({saveRoot:"/savesth075",scoreFile:"score.dat",configFiles:Object.freeze(["config.dat"])}),
        runtime: "./runtime/th075/th075.html", inputProfile: "fighting",
        touchMovementModes: Object.freeze(["joystick","joystick-free"]),
        touchActions: Object.freeze({
            fire: Object.freeze({label:"A",descriptionKey:"touch.fighting.light",bit:1,code:"KeyZ",key:"z",keyCode:90}),
            bomb: Object.freeze({label:"C",descriptionKey:"touch.fighting.projectile",bit:4,code:"KeyC",key:"c",keyCode:67}),
            focus: Object.freeze({label:"B",descriptionKey:"touch.fighting.heavy",bit:2,code:"KeyX",key:"x",keyCode:88}),
            function: Object.freeze({label:"D",descriptionKey:"touch.fighting.dash",bit:8,code:"KeyV",key:"v",keyCode:86}),
        }),
        touchFire: Object.freeze({mode:"held-key",labelKey:"touch.actionHold",key:Object.freeze({code:"KeyZ",key:"z",keyCode:90})}),
        musicCapabilities: Object.freeze({midi:false}),
        musicRuntime: Object.freeze({localOggConfigureMode:"runtime-selection",requiresCompleteOgg:true}),
        support: Object.freeze({highRefreshRate:false,sourceRepository:"https://github.com/Goan114/th075",adaptationNotice:"early-test"}),
        runtimeFileLayout: "directory", requiredShared: Object.freeze([]),
        runtimeAssets: Object.freeze(["th075.html","manifest.json","shell.mjs","managed.css","keyboard.mjs","directory-keyboard.mjs","display.mjs","content-bundle.mjs","save-sync.mjs","th075.mjs","th075.wasm"]),
        dataProvider: "retail-memory",
        package: Object.freeze({dataFileId:"game-data",dataTarget:"/th075.data",maxImportBytes:512 * 1024 * 1024,
            musicSourceDirectories:Object.freeze({ogg:"music"}),musicMounts:Object.freeze({ogg:"/music"})}),
        replay: Object.freeze({prefix:"th075",extensions:Object.freeze(["rep"]),naming:"filename"}),
        multiplayerRuntime: "./runtime/th075/multiplayer/th075.html",
        multiplayer: Object.freeze({
            titleKey:"game.title.th075mp", gameplay:"versus",
            inputTiming:Object.freeze({rollbackLimit:8,sendPredictionLimit:8,measuredStartup:true,manualDelayLimit:9}),
            playerCounts:Object.freeze([2]), difficulties:TH09_MULTIPLAYER_DIFFICULTIES,
            loadouts:TH075_MULTIPLAYER_LOADOUTS, peerTransportGlobal:"__th075PeerTransport",
            loadoutFields:Object.freeze([
                Object.freeze({labelKey:"multiplayer.character"}),
                Object.freeze({labelKey:"multiplayer.firstSpell",dependsOn:Object.freeze([0])}),
                Object.freeze({labelKey:"multiplayer.secondSpell",dependsOn:Object.freeze([0])}),
            ]),
            spectator:true, titleRoomEntry:false,
        }),
        features: Object.freeze({thprac:false,languages:true,focusHitbox:false}),
    }),
    th08: Object.freeze({
        thpracExtraFunctionKeys: Object.freeze(["U"]),
        cardArtwork: "th08-card.webp",
        number: "08",
        title: "東方永夜抄",
        subtitle: "Imperishable Night",
        storage: Object.freeze({
            saveRoot: "/savesth08",
            scoreFile: "score.dat",
            configFiles: Object.freeze(["th08.cfg"]),
        }),
        runtime: "./runtime/th08/th08.html",
        musicCapabilities: Object.freeze({ midi: true }),
        musicRuntime: Object.freeze({ localOggConfigureMode: "midi-sentinel" }),
        touchFire: TOGGLE_TOUCH_FIRE,
        support: Object.freeze({ highRefreshRate: true,
            sourceRepository: "https://github.com/YomotsuHisami/th08",
            adaptationNotice: "early-test",
        }),
        runtimeFileLayout: "directory",
        requiredShared: Object.freeze(["/msgothic.ttc", "/unifont.otf"]),
        runtimeAssets: Object.freeze([
            "th08.html",
            "manifest.json",
            "shell.mjs",
            "directory-keyboard.mjs",
            "eagler-host.mjs",
            "practice.mjs",
            "practice-config.mjs",
            "practice-sections.mjs",
            "motion-replay.mjs",
            "th08-sdl.mjs",
            "th08-sdl.wasm",
            "resources.json",
            "fonts/blend.bin",
            "fonts/cp932.bin",
        ]),
        dataProvider: "retail-memory",
        package: Object.freeze({
            dataFileId: "game-data",
            dataTarget: "/th08.data",
            rawDataImport: Object.freeze({ fileNames: Object.freeze(["th08.dat"]) }),
            musicSourceDirectories: Object.freeze({ ogg: "bgm-ogg" }),
            musicMounts: Object.freeze({ ogg: "/bgm-ogg" }),
        }),
        replay: Object.freeze({ prefix: "th8" }),
        multiplayerRuntime: "./runtime/th08/multiplayer/th08.html",
        multiplayer: Object.freeze({
            titleKey: "game.title.th08mp",
            gameplay: "cooperative",
            inputTiming: Object.freeze({ rollbackLimit: 8, sendPredictionLimit: 8, measuredStartup: true }),
            playerCounts: STANDARD_MULTIPLAYER_PLAYER_COUNTS,
            difficulties: STANDARD_MULTIPLAYER_DIFFICULTIES,
            loadouts: TH08_MULTIPLAYER_LOADOUTS,
            peerTransportGlobal: "__th08PeerTransport",
            preflightWithoutRoom: true,
            spectator: true,
            titleRoomEntry: false,
        }),
        features: Object.freeze({ thprac: true, languages: true, focusHitbox: false }),
    }),
    th09: Object.freeze({
        cardArtwork: "th09-card.webp",
        number: "09",
        title: "東方花映塚",
        subtitle: "Phantasmagoria of Flower View",
        storage: Object.freeze({ saveRoot: "/savesth09", scoreFile: "score.dat", configFiles: Object.freeze(["th09.cfg"]) }),
        runtime: "./runtime/th09/th09.html",
        musicCapabilities: Object.freeze({ midi: false }),
        musicRuntime: Object.freeze({ localOggConfigureMode: "runtime-selection" }),
        touchFire: CHARGE_TOUCH_FIRE,
        support: Object.freeze({ highRefreshRate: false, sourceRepository: "https://github.com/YomotsuHisami/th09", adaptationNotice: "early-test" }),
        runtimeFileLayout: "directory",
        requiredShared: Object.freeze(["/msgothic.ttc", "/unifont.otf"]),
        runtimeAssets: Object.freeze([
            "th09.html", "manifest.json", "version.json", "shell.mjs", "managed.css", "keyboard.mjs",
            "directory-keyboard.mjs",
            "shared-netplay.mjs", "motion-replay.mjs", "auto-replay.mjs", "replay-player.mjs", "playback-clock.mjs", "th09.mjs", "th09.wasm",
            "fonts/blend.bin", "fonts/cp932.bin",
        ]),
        dataProvider: "retail-memory",
        package: Object.freeze({
            dataFileId: "game-data", dataTarget: "/th09.data",
            rawDataImport: Object.freeze({ fileNames: Object.freeze(["th09.dat"]) }),
            musicSourceDirectories: Object.freeze({ ogg: "music" }),
            musicMounts: Object.freeze({ ogg: "/music" }),
        }),
        replay: Object.freeze({ prefix: "th9" }),
        multiplayerRuntime: "./runtime/th09/multiplayer/th09.html",
        multiplayer: Object.freeze({
            titleKey: "game.title.th09mp",
            gameplay: "versus",
            inGameSelection: true,
            inputTiming: Object.freeze({ rollbackLimit: 8, measuredStartup: true, manualDelayLimit: 9 }),
            playerCounts: Object.freeze([2]),
            difficulties: TH09_MULTIPLAYER_DIFFICULTIES,
            loadouts: TH09_MULTIPLAYER_LOADOUTS,
            peerTransportGlobal: "__th09PeerTransport",
            // TH09's two-player rollback session publishes confirmed inputs to
            // admitted spectators without reviving the deprecated lockstep path.
            spectator: true,
            titleRoomEntry: true,
        }),
        features: Object.freeze({ thprac: false, languages: true, focusHitbox: false }),
    }),
    th10: Object.freeze({
        cardArtwork: "th10-card.webp",
        number: "10",
        title: "東方風神録",
        subtitle: "Mountain of Faith",
        storage: Object.freeze({ saveRoot: "/savesth10", scoreFile: "scoreth10.dat", configFiles: Object.freeze(["th10.cfg"]),
            hintFiles: Object.freeze(["hint/hint_user.txt", "hint/hint_auto.txt"]),
        }),
        display: Object.freeze({ faithBar: true }),
        runtime: "./runtime/th10/th10.html",
        thpracExtraFunctionKeys: Object.freeze(["U"]),
        musicCapabilities: Object.freeze({ midi: false }),
        musicRuntime: Object.freeze({ localOggConfigureMode: "midi-sentinel" }),
        touchFire: TOGGLE_TOUCH_FIRE,
        support: Object.freeze({ highRefreshRate: true,
            sourceRepository: "https://github.com/YomotsuHisami/th10",
            adaptationNotice: "early-test",
        }),
        runtimeFileLayout: "directory",
        requiredShared: Object.freeze(["/msgothic.ttc", "/unifont.otf"]),
        runtimeAssets: Object.freeze([
            "th10.html",
            "manifest.json",
            "shell.mjs",
            "directory-keyboard.mjs",
            "eagler-host.mjs",
            "practice.mjs",
            "practice-config.mjs",
            "practice-sections.mjs",
            "motion-replay.mjs",
            "th10-sdl.mjs",
            "th10-sdl.wasm",
            "resources.json",
            "fonts/blend.bin",
            "fonts/codepages.bin",
        ]),
        dataProvider: "retail-memory",
        package: Object.freeze({
            dataFileId: "game-data", dataTarget: "/th10.data",
            musicSourceDirectories: Object.freeze({ ogg: "bgm-ogg" }),
            musicMounts: Object.freeze({ ogg: "/bgm-ogg" }),
        }),
        replay: Object.freeze({ prefix: "th10" }),
        multiplayerRuntime: "./runtime/th10/multiplayer/th10.html",
        multiplayer: Object.freeze({
            titleKey: "game.title.th10mp",
            gameplay: "cooperative",
            inputTiming: Object.freeze({ rollbackLimit: 12, measuredStartup: true }),
            playerCounts: STANDARD_MULTIPLAYER_PLAYER_COUNTS,
            difficulties: STANDARD_MULTIPLAYER_DIFFICULTIES,
            loadouts: TH10_MULTIPLAYER_LOADOUTS,
            peerTransportGlobal: "__th10PeerTransport",
            spectator: true,
            titleRoomEntry: false,
        }),
        features: Object.freeze({ thprac: true, languages: true, focusHitbox: false }),
    }),
    th11: Object.freeze({
        number: "11",
        title: "東方地霊殿",
        subtitle: "Subterranean Animism",
        cardArtwork: "th11-card.webp",
        storage: Object.freeze({
            saveRoot: "/savesth11",
            scoreFile: "scoreth11.dat",
            configFiles: Object.freeze(["th11.cfg"]),
        }),
        runtime: "./runtime/th11/th11.html",
        musicCapabilities: Object.freeze({ midi: false }),
        musicRuntime: Object.freeze({ localOggConfigureMode: "runtime-selection" }),
        touchFire: TOGGLE_TOUCH_FIRE,
        support: Object.freeze({ highRefreshRate: true,
            sourceRepository: "https://github.com/YomotsuHisami/th11",
            adaptationNotice: "early-test",
        }),
        runtimeFileLayout: "directory",
        // Original glyphs are rasterized on demand from the shared Gothic font.
        requiredShared: Object.freeze(["/msgothic.ttc", "/unifont.otf"]),
        runtimeAssets: Object.freeze([
            "th11.html",
            "manifest.json",
            "shell.mjs",
            "managed.css",
            "keyboard.mjs",
            "directory-keyboard.mjs",
            "eagler-host.mjs",
            "motion-replay.mjs",
            "th11-sdl.mjs",
            "th11-sdl.wasm",
            "resources.json",
            "fonts/cp932.bin",
        ]),
        dataProvider: "retail-memory",
        package: Object.freeze({
            dataFileId: "game-data",
            dataTarget: "/th11.dat",
            rawDataImport: Object.freeze({ fileNames: Object.freeze(["th11.dat"]) }),
            musicSourceDirectories: Object.freeze({ ogg: "music" }),
            musicMounts: Object.freeze({ ogg: "/music" }),
        }),
        replay: Object.freeze({ prefix: "th11" }),
        multiplayerRuntime: "./runtime/th11/multiplayer/th11.html",
        multiplayer: Object.freeze({
            titleKey: "game.title.th11mp",
            gameplay: "cooperative",
            inputTiming: Object.freeze({ rollbackLimit: 12, sendPredictionLimit: 12, measuredStartup: true, manualDelayLimit: 9 }),
            playerCounts: STANDARD_MULTIPLAYER_PLAYER_COUNTS,
            difficulties: STANDARD_MULTIPLAYER_DIFFICULTIES,
            loadouts: TH11_MULTIPLAYER_LOADOUTS,
            peerTransportGlobal: "__th11PeerTransport",
            spectator: true,
            titleRoomEntry: false,
        }),
        features: Object.freeze({ thprac: true, languages: true, focusHitbox: false }),
    }),
    th12: Object.freeze({
        number: "12", title: "東方星蓮船", subtitle: "Undefined Fantastic Object",
        cardArtwork: "th12-card.webp",
        storage: Object.freeze({saveRoot: "/savesth12", scoreFile: "scoreth12.dat", configFiles: Object.freeze(["th12.cfg"])}),
        runtime: "./runtime/th12/th12.html",
        runtimeFileLayout: "directory",
        runtimeAssets: Object.freeze(["th12.html", "manifest.json", "game.css", "game.mjs", "core.mjs", "core.wasm", "managed-data.mjs", "managed-shell.mjs", "storage.mjs"]),
        requiredShared: Object.freeze(["/msgothic.ttc", "/unifont.otf"]),
        dataProvider: "retail-memory",
        runtimeResourceTransport: "resources",
        package: Object.freeze({dataFileId: "game-data", dataTarget: "/th12.data",
            musicSourceDirectories: Object.freeze({ogg: "music"}), musicMounts: Object.freeze({ogg: "/music"})}),
        musicCapabilities: Object.freeze({midi: false}),
        musicRuntime: Object.freeze({localOggConfigureMode: "runtime-selection"}),
        touchFire: TOGGLE_TOUCH_FIRE,
        support: Object.freeze({highRefreshRate: true, sourceRepository: "https://github.com/Tweet-META/th12", adaptationNotice: "early-test"}),
        replay: Object.freeze({prefix: "th12"}),
        multiplayerRuntime: "./runtime/th12/multiplayer/th12.html",
        multiplayer: Object.freeze({
            titleKey: "game.title.th12mp", gameplay: "cooperative",
            inputTiming: Object.freeze({ rollbackLimit: 12, sendPredictionLimit: 12, measuredStartup: true, manualDelayLimit: 9 }),
            playerCounts: STANDARD_MULTIPLAYER_PLAYER_COUNTS, difficulties: STANDARD_MULTIPLAYER_DIFFICULTIES,
            loadouts: TH12_MULTIPLAYER_LOADOUTS, peerTransportGlobal: "__th12PeerTransport",
            spectator: true, titleRoomEntry: false,
        }),
        features: Object.freeze({thprac: true, languages: true, focusHitbox: false}),
    }),
    th13: Object.freeze({
        testOnly: true,
        buildOptIn: true,
        number: "13",
        title: "東方神霊廟",
        subtitle: "Ten Desires",
        cardArtwork: "th13-card.webp",
        storage: Object.freeze({
            saveRoot: "/savesth13",
            scoreFile: "scoreth13.dat",
            configFiles: Object.freeze([]),
        }),
        runtime: "./runtime/th13/th13.html",
        musicCapabilities: Object.freeze({ midi: false }),
        musicRuntime: Object.freeze({ localOggConfigureMode: "runtime-selection" }),
        touchFire: TOGGLE_TOUCH_FIRE,
        support: Object.freeze({
            highRefreshRate: false,
            sourceRepository: "https://github.com/AgentMystia/th13",
            adaptationNotice: "early-test",
        }),
        runtimeFileLayout: "directory",
        requiredShared: Object.freeze(["/msgothic.ttc", "/unifont.otf"]),
        runtimeAssets: Object.freeze([
            "th13.html",
            "manifest.json",
            "shell.mjs",
            "managed.css",
            "keyboard.mjs",
            "directory-keyboard.mjs",
            "runtime-assets.mjs",
            "startup-branding.mjs",
            "th13-sdl.mjs",
            "th13-sdl.wasm",
            "resources.json",
        ]),
        dataProvider: "retail-memory",
        package: Object.freeze({
            dataFileId: "game-data",
            dataTarget: "/th13.data",
            musicSourceDirectories: Object.freeze({ ogg: "bgm-ogg" }),
            musicMounts: Object.freeze({ ogg: "/assets" }),
        }),
        replay: Object.freeze({ prefix: "th13" }),
        multiplayerRuntime: "./runtime/th13/multiplayer/th13.html",
        multiplayer: Object.freeze({
            titleKey: "game.title.th13mp", gameplay: "cooperative",
            inputTiming: Object.freeze({ rollbackLimit: 12, sendPredictionLimit: 12, measuredStartup: true, manualDelayLimit: 9 }),
            playerCounts: STANDARD_MULTIPLAYER_PLAYER_COUNTS, difficulties: STANDARD_MULTIPLAYER_DIFFICULTIES,
            loadouts: TH13_MULTIPLAYER_LOADOUTS, peerTransportGlobal: "__th13PeerTransport",
            spectator: true, titleRoomEntry: false,
        }),
        features: Object.freeze({ thprac: false, languages: false, focusHitbox: false }),
    }),
    th15: Object.freeze({
        number: "15",
        title: "東方紺珠伝",
        subtitle: "Legacy of Lunatic Kingdom",
        cardArtwork: "th15-card.webp",
        storage: Object.freeze({
            saveRoot: "/savesth15",
            scoreFile: "scoreth15.dat",
            configFiles: Object.freeze(["th15.cfg"]),
        }),
        runtime: "./runtime/th15/th15.html",
        musicCapabilities: Object.freeze({ midi: false }),
        musicRuntime: Object.freeze({ localOggConfigureMode: "runtime-selection" }),
        touchFire: TOGGLE_TOUCH_FIRE,
        thpracExtraFunctionKeys: Object.freeze(["U"]),
        support: Object.freeze({ highRefreshRate: true,
            sourceRepository: "https://github.com/YomotsuHisami/th15",
            adaptationNotice: "early-test",
        }),
        runtimeFileLayout: "directory",
        // Original glyphs are rasterized on demand; localization uses Unicode fonts.
        requiredShared: Object.freeze(["/msgothic.ttc", "/unifont.otf"]),
        runtimeAssets: Object.freeze([
            "th15.html",
            "manifest.json",
            "shell.mjs",
            "managed.css",
            "keyboard.mjs",
            "directory-keyboard.mjs",
            "th15.mjs",
            "th15.wasm",
            "resources.json",
            "fonts/cp932.bin",
        ]),
        dataProvider: "retail-memory",
        package: Object.freeze({
            dataFileId: "game-data",
            dataTarget: "/th15.dat",
            rawDataImport: Object.freeze({ fileNames: Object.freeze(["th15.dat"]) }),
            musicSourceDirectories: Object.freeze({ ogg: "music" }),
            musicMounts: Object.freeze({ ogg: "/music" }),
        }),
        replay: Object.freeze({ prefix: "th15" }),
        multiplayerRuntime: "./runtime/th15/multiplayer/th15.html",
        multiplayer: Object.freeze({
            titleKey: "game.title.th15mp",
            gameplay: "cooperative",
            inputTiming: Object.freeze({ rollbackLimit: 12, sendPredictionLimit: 12, measuredStartup: true, manualDelayLimit: 9 }),
            playerCounts: STANDARD_MULTIPLAYER_PLAYER_COUNTS,
            difficulties: STANDARD_MULTIPLAYER_DIFFICULTIES,
            loadouts: TH15_MULTIPLAYER_LOADOUTS,
            peerTransportGlobal: "__th15PeerTransport",
            spectator: true,
            titleRoomEntry: false,
        }),
        features: Object.freeze({ thprac: true, languages: true, focusHitbox: false }),
    }),
    th17: Object.freeze({
        testOnly: true,
        number: "17", title: "東方鬼形獣", subtitle: "Wily Beast and Weakest Creature", cardArtwork: "th17-card.webp",
        renderScales: Object.freeze([2, 1]),
        thpracRuntime: Object.freeze({session: "native"}),
        storage: Object.freeze({saveRoot: "/savesth17", scoreFile: "scoreth17.dat", configFiles: Object.freeze(["th17.cfg"])}),
        runtime: "./runtime/th17/th17.html",
        musicCapabilities: Object.freeze({midi: false}),
        musicRuntime: Object.freeze({localOggConfigureMode: "runtime-selection"}),
        touchFire: TOGGLE_TOUCH_FIRE,
        support: Object.freeze({highRefreshRate: false, sourceRepository: "https://github.com/Goan114/eagler-touhou", adaptationNotice: "early-test"}),
        runtimeFileLayout: "directory", requiredShared: Object.freeze(["/msgothic.ttc", "/unifont.otf"]),
        runtimeAssets: Object.freeze(["th17.html", "manifest.json", "shell.mjs", "managed.css", "keyboard.mjs", "directory-keyboard.mjs", "direct-input-bridge.mjs", "gpu-timing.mjs", "th17.mjs", "th17.wasm", "resources.json", "fonts/cp932.bin", "local-replay.mjs", "local-replay-native.mjs", "spectator-playback.mjs"]),
        dataProvider: "retail-memory",
        package: Object.freeze({dataFileId: "game-data", dataTarget: "/th17.dat",
            rawDataImport: Object.freeze({fileNames: Object.freeze(["th17.dat"])}),
            musicSourceDirectories: Object.freeze({ogg: "music"}), musicMounts: Object.freeze({ogg: "/music"})}),
        replay: Object.freeze({prefix: "th17"}),
        multiplayerRuntime: "./runtime/th17/multiplayer/th17.html",
        multiplayer: Object.freeze({
            titleKey: "game.title.th17mp", gameplay: "cooperative",
            playerCounts: STANDARD_MULTIPLAYER_PLAYER_COUNTS, difficulties: STANDARD_MULTIPLAYER_DIFFICULTIES,
            loadouts: TH17_MULTIPLAYER_LOADOUTS, peerTransportGlobal: "__th17PeerTransport",
            loadoutFields: Object.freeze([
                Object.freeze({labelKey:"multiplayer.character"}),
                Object.freeze({labelKey:"multiplayer.animalSpirit"}),
            ]),
            spectator: true, titleRoomEntry: false,
            inputTiming: Object.freeze({rollbackLimit: 12, sendPredictionLimit: 12, manualDelayLimit: 9}),
        }),
        thpracExtraFunctionKeys: Object.freeze(["U"]),
        features: Object.freeze({thprac: true, languages: true, focusHitbox: false}),
    }),
    th19: Object.freeze({
        testOnly: true,
        number: "19", title: "東方獣王園", subtitle: "Unfinished Dream of All Living Ghost",
        storage: Object.freeze({saveRoot: "/savesth19", scoreFile: "scoreth19.dat", configFiles: Object.freeze(["th19.cfg"])}),
        runtime: "./runtime/th19/th19.html", runtimeFileLayout: "directory", dataProvider: "retail-memory",
        requiredShared: Object.freeze(["/msgothic.ttc"]),
        runtimeAssets: Object.freeze(["th19.html","manifest.json","resource-integrity.mjs","shell.mjs","managed.css","keyboard.mjs","directory-keyboard.mjs","keyboard-owners.mjs","keyboard-input-bridge.mjs","netplay.mjs","peer-socket.mjs","rollback-session.mjs","native-adapter.mjs","th19-recording.mjs","replay-session.mjs","th19.mjs","th19.wasm"]),
        musicCapabilities: Object.freeze({midi: false}), musicRuntime: Object.freeze({localOggConfigureMode: "runtime-selection"}),
        touchFire: TOGGLE_TOUCH_FIRE,
        support: Object.freeze({highRefreshRate: false, sourceRepository: "https://github.com/Ritosa2333/eagler-touhou", adaptationNotice: "early-test"}),
        package: Object.freeze({dataFileId: "game-data",dataTarget: "/th19.dat",rawDataImport: Object.freeze({fileNames: Object.freeze(["th19.dat"])}),musicSourceDirectories: Object.freeze({ogg: "music"}),musicMounts: Object.freeze({ogg: "/music"})}),
        replay: Object.freeze({prefix: "th19"}),
        multiplayerRuntime: "./runtime/th19/multiplayer/th19.html",
        multiplayer: Object.freeze({titleKey: "game.title.th19mp",gameplay: "versus",playerCounts: Object.freeze([2]),difficulties: TH19_MULTIPLAYER_DIFFICULTIES,
            loadouts: Object.freeze(TH19_CHARACTERS.map((glyph,character)=>Object.freeze({labelKey:`multiplayer.loadout.th19_${character}`,glyph,character,shot:0}))),
            inputTiming: Object.freeze({rollbackLimit:8,sendPredictionLimit:8,manualDelayLimit:8}),peerTransportGlobal:"__th19PeerTransport",spectator:true,titleRoomEntry:false,inGameCharacterSelection:true}),
        features: Object.freeze({thprac:false,languages:false,focusHitbox:false}),
    }),
    th20: Object.freeze({
        testOnly: true,
        number: "20",
        title: "東方錦上京",
        subtitle: "Fossilized Wonders",
        cardArtwork: "th20-card.webp",
        storage: Object.freeze({
            saveRoot: "/savesth20",
            scoreFile: "scoreth20.dat",
            configFiles: Object.freeze(["th20.cfg"]),
        }),
        runtime: "./runtime/th20/th20.html",
        musicCapabilities: Object.freeze({ midi: false }),
        musicRuntime: Object.freeze({ localOggConfigureMode: "runtime-selection" }),
        touchFire: TOGGLE_TOUCH_FIRE,
        support: Object.freeze({ highRefreshRate: false,
            sourceRepository: "https://github.com/Goan114/touhou20",
            adaptationNotice: "early-test",
            credit: Object.freeze({ name: "ぃ尐懒猫ゞ", url: "https://space.bilibili.com/15669619" }),
        }),
        runtimeFileLayout: "directory",
        requiredShared: Object.freeze(["/msgothic.ttc", "/unifont.otf"]),
        runtimeAssets: Object.freeze([
            "th20.html",
            "manifest.json",
            "shell.mjs",
            "directory-keyboard.mjs",
            "eagler-host.mjs",
            "motion-replay.mjs",
            "th20-sdl.mjs",
            "th20-sdl.wasm",
            "resources.json",
        ]),
        dataProvider: "retail-memory",
        package: Object.freeze({
            dataFileId: "game-data",
            dataTarget: "/th20.data",
            // TH20's canonical BGM is the /bgm-ogg OGG set decoded by the Runtime;
            // retail thbgm.dat is no longer a shipped resource.
            musicSourceDirectories: Object.freeze({ ogg: "bgm-ogg" }),
            musicMounts: Object.freeze({ ogg: "/bgm-ogg" }),
            rawDataImport: Object.freeze({ fileNames: Object.freeze(["th20.dat"]) }),
        }),
        replay: Object.freeze({ prefix: "th20" }),
        features: Object.freeze({ thprac: false, languages: true, focusHitbox: false }),
    }),
});
/** Interpret the same product timing declaration at every shared boundary. */
export function multiplayerInputTimingPolicy(product) {
    const timing = product?.inputTiming;
    const measuredStartup = timing?.measuredStartup === true;
    const rollback = timing?.rollbackLimit !== 0;
    return {
        measuredStartup,
        rollback,
        manualDelayLimit: timing?.manualDelayLimit ?? (measuredStartup ? 9 : 8),
        predictionLimit: rollback ? timing?.sendPredictionLimit ?? 8 : 0,
    };
}
const productGameEntries = Object.entries(PRODUCT_GAMES);
const multiplayerProductGames = Object.freeze(Object.fromEntries(productGameEntries
    .filter((entry) => "multiplayerRuntime" in entry[1] && "multiplayer" in entry[1])
    .map(([gameId]) => [`${gameId}mp`, gameId])));
export const PRODUCT_IDS = Object.freeze([
    ...Object.keys(PRODUCT_GAMES),
    ...Object.keys(multiplayerProductGames),
]);
// Default navigation policy belongs with the product registry, not Launcher
// flow code. Keep today's behavior explicit so adding another product never
// requires hunting for fallback game IDs in routing/lobby logic.
export const DEFAULT_PRODUCT_ID = "th06";
export const DEFAULT_MULTIPLAYER_PRODUCT_ID = "th07mp";
export function isGameId(value) {
    return Object.hasOwn(PRODUCT_GAMES, value);
}
export function productRequiresImport(gameId) {
    return "importOnly" in PRODUCT_GAMES[gameId];
}

export function productEnabledForBuild(productId, testBuild = false) {
    if (!isProductId(productId))
        return false;
    const game = PRODUCT_GAMES[gameIdForProduct(productId)];
    if ("hidden" in game && game.hidden)
        return false;
    return !("testOnly" in game && game.testOnly) || testBuild === true;
}
export function isProductId(value) {
    return PRODUCT_IDS.includes(value);
}
export function isMultiplayerProductId(productId) {
    return Object.hasOwn(multiplayerProductGames, productId);
}
export function gameIdForProduct(productId) {
    return isMultiplayerProductId(productId) ? multiplayerProductGames[productId] ?? productId : productId;
}
export function multiplayerProductIdForGame(gameId) {
    const productId = `${gameId}mp`;
    return isMultiplayerProductId(productId) ? productId : null;
}
export function multiplayerConfigForProduct(productId) {
    const gameId = gameIdForProduct(productId);
    if (!isGameId(gameId))
        return null;
    const product = PRODUCT_GAMES[gameId];
    return "multiplayer" in product
        ? product.multiplayer
        : null;
}
const HOST_RUNTIME_FEATURES = new Set(HOST_RUNTIME_FEATURE_IDS);
// Static product policy is the capability ceiling. A Host may report that its
// concrete Runtime lacks an attested Runtime feature, but it cannot enable a
// feature the product does not support or override Launcher-owned features.
// Missing Host fields preserve host-manifest/1 deployments from before Runtime
// capability attestation became explicit.
export function productFeatureAvailable(gameId, featureId, hostFeatures = null) {
    const supported = isGameId(gameId) && PRODUCT_GAMES[gameId].features[featureId] === true;
    if (!supported || !HOST_RUNTIME_FEATURES.has(featureId))
        return supported;
    const hosted = hostFeatures?.[featureId];
    return hosted == null ? true : hosted === true;
}
export function createLocalProductManifest() {
    return {
        protocol: HOST_PROTOCOL,
        shared: { resourceMode: "hosted", testBuild: false },
        games: Object.fromEntries(productGameEntries.map(([game, product]) => [game, {
                ...product,
                music: { midi: product.musicCapabilities.midi ? { files: [] } : { files: [], supported: false } },
                languageOptions: [{ id: "ja", title: "日本語(原版)", pack: null }],
            }])),
    };
}
