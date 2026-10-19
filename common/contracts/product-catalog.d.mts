export declare const HOST_PROTOCOL = "eagler-touhou/1";
export declare const PRODUCT_FEATURE_POLICY: Readonly<{
    readonly thprac: Readonly<{
        runtimeRelease: true;
        hostManifestFeature: true;
        hostSurface: "features.thprac";
        hostValueSource: "server-request-and-runtime";
    }>;
    readonly languages: Readonly<{
        runtimeRelease: true;
        hostManifestFeature: false;
        hostSurface: "languages + languageOptions";
        hostValueSource: "language-catalog";
    }>;
    readonly focusHitbox: Readonly<{
        runtimeRelease: true;
        hostManifestFeature: true;
        hostSurface: "features.focusHitbox";
        hostValueSource: "runtime-release";
    }>;
}>;
export type ProductFeatureId = keyof typeof PRODUCT_FEATURE_POLICY;
export declare const PRODUCT_FEATURE_IDS: readonly ("focusHitbox" | "languages" | "thprac")[];
export type HostRuntimeFeatureId = {
    [K in ProductFeatureId]: (typeof PRODUCT_FEATURE_POLICY)[K]["hostManifestFeature"] extends true ? K : never;
}[ProductFeatureId];
export declare const HOST_RUNTIME_FEATURE_IDS: readonly HostRuntimeFeatureId[];
export type HostRuntimeFeatures = Readonly<Partial<Record<HostRuntimeFeatureId, boolean>>>;
export declare function isHostRuntimeFeatureId(value: string): value is HostRuntimeFeatureId;
export declare function languagePriority(id: string): number;
export declare const PRODUCT_GAMES: Readonly<{
    th06: Readonly<{
        cardArtwork: "th06-card.webp";
        number: "06";
        title: "東方紅魔郷";
        subtitle: "the Embodiment of Scarlet Devil";
        storage: Readonly<{
            saveRoot: "/savesth06";
            scoreFile: "score.dat";
            configFiles: readonly string[];
        }>;
        runtime: "./runtime/th06/th06.html";
        musicCapabilities: Readonly<{
            midi: true;
        }>;
        musicRuntime: Readonly<{
            localOggConfigureMode: "midi-sentinel";
        }>;
        touchFire: Readonly<{
            readonly mode: "toggle";
            readonly labelKey: "touch.tapToggle";
        }>;
        support: Readonly<{
            highRefreshRate: true;
            sourceRepository: "https://github.com/YomotsuHisami/th06";
        }>;
        dataProvider: "emscripten-preload";
        package: Readonly<{
            dataFileId: "game-data";
            dataTarget: "/th06.data";
            musicSourceDirectories: Readonly<{
                wav: "bgm";
                ogg: "bgm";
            }>;
            musicMounts: Readonly<{
                wav: "/bgm";
                ogg: "/bgm";
            }>;
        }>;
        replay: Readonly<{
            prefix: "th6";
        }>;
        multiplayerRuntime: "./runtime/th06/multiplayer/th06.html";
        multiplayer: Readonly<{
            titleKey: "game.title.th06mp";
            gameplay: "cooperative";
            playerCounts: readonly [2, 3];
            difficulties: readonly string[];
            loadouts: readonly (Readonly<{
                labelKey: "multiplayer.loadout.reimuA";
                glyph: "霊";
                character: 0;
                shot: 0;
            }> | Readonly<{
                labelKey: "multiplayer.loadout.reimuB";
                glyph: "霊";
                character: 0;
                shot: 1;
            }> | Readonly<{
                labelKey: "multiplayer.loadout.marisaA";
                glyph: "魔";
                character: 1;
                shot: 0;
            }> | Readonly<{
                labelKey: "multiplayer.loadout.marisaB";
                glyph: "魔";
                character: 1;
                shot: 1;
            }>)[];
            peerTransportGlobal: "__th06PeerTransport";
            spectator: true;
            titleRoomEntry: false;
        }>;
        features: Readonly<{
            thprac: true;
            languages: true;
            focusHitbox: true;
        }>;
    }>;
    th07: Readonly<{
        cardArtwork: "th07-card.webp";
        cardPresentation: Readonly<{
            positionPercent: 58;
            artBrightness: 0.42;
            artSaturation: 0.62;
            glowBrightness: 0.8;
            glowSaturation: 0.88;
        }>;
        number: "07";
        title: "東方妖々夢";
        subtitle: "Perfect Cherry Blossom";
        storage: Readonly<{
            saveRoot: "/savesth07";
            scoreFile: "score.dat";
            configFiles: readonly string[];
        }>;
        runtime: "./runtime/th07/th07.html";
        musicCapabilities: Readonly<{
            midi: true;
        }>;
        musicRuntime: Readonly<{
            localOggConfigureMode: "midi-sentinel";
        }>;
        touchFire: Readonly<{
            readonly mode: "toggle";
            readonly labelKey: "touch.tapToggle";
        }>;
        support: Readonly<{
            highRefreshRate: true;
            sourceRepository: "https://github.com/YomotsuHisami/th07";
        }>;
        dataProvider: "emscripten-preload";
        package: Readonly<{
            dataFileId: "game-data";
            dataTarget: "/th07.data";
            musicSourceDirectories: Readonly<{
                wav: ".";
                ogg: "bgm-ogg";
            }>;
            musicMounts: Readonly<{
                wav: "/";
                ogg: "/bgm-ogg";
            }>;
        }>;
        replay: Readonly<{
            prefix: "th7";
        }>;
        multiplayerRuntime: "./runtime/th07/multiplayer/th07.html";
        multiplayer: Readonly<{
            titleKey: "game.title.th07mp";
            gameplay: "cooperative";
            playerCounts: readonly [2, 3];
            difficulties: readonly string[];
            loadouts: readonly (Readonly<{
                labelKey: "multiplayer.loadout.reimuA";
                glyph: "霊";
                character: 0;
                shot: 0;
            }> | Readonly<{
                labelKey: "multiplayer.loadout.reimuB";
                glyph: "霊";
                character: 0;
                shot: 1;
            }> | Readonly<{
                labelKey: "multiplayer.loadout.marisaA";
                glyph: "魔";
                character: 1;
                shot: 0;
            }> | Readonly<{
                labelKey: "multiplayer.loadout.marisaB";
                glyph: "魔";
                character: 1;
                shot: 1;
            }> | Readonly<{
                labelKey: "multiplayer.loadout.sakuyaA";
                glyph: "咲";
                character: 2;
                shot: 0;
            }> | Readonly<{
                labelKey: "multiplayer.loadout.sakuyaB";
                glyph: "咲";
                character: 2;
                shot: 1;
            }>)[];
            peerTransportGlobal: "__th07PeerTransport";
            spectator: true;
            titleRoomEntry: false;
        }>;
        features: Readonly<{
            thprac: true;
            languages: true;
            focusHitbox: false;
        }>;
    }>;
    th075: Readonly<{
        number: "07.5"; title: "東方萃夢想"; subtitle: "Immaterial and Missing Power"; cardArtwork: "th075-card.webp";
        storage: Readonly<{saveRoot:"/savesth075";scoreFile:"score.dat";configFiles:readonly string[]}>;
        runtime: "./runtime/th075/th075.html"; inputProfile: "fighting";
        touchMovementModes: readonly ["joystick", "joystick-free"];
        touchActions: Readonly<{
            fire: Readonly<{label:"A";descriptionKey:"touch.fighting.light";bit:1;code:"KeyZ";key:"z";keyCode:90}>;
            bomb: Readonly<{label:"C";descriptionKey:"touch.fighting.projectile";bit:4;code:"KeyC";key:"c";keyCode:67}>;
            focus: Readonly<{label:"B";descriptionKey:"touch.fighting.heavy";bit:2;code:"KeyX";key:"x";keyCode:88}>;
            function: Readonly<{label:"D";descriptionKey:"touch.fighting.dash";bit:8;code:"KeyV";key:"v";keyCode:86}>;
        }>;
        touchFire: Readonly<{mode:"held-key";labelKey:"touch.actionHold";key:Readonly<{code:"KeyZ";key:"z";keyCode:90}>}>;
        musicCapabilities: Readonly<{midi:false}>;
        musicRuntime: Readonly<{localOggConfigureMode:"runtime-selection";requiresCompleteOgg:true}>;
        support: Readonly<{highRefreshRate:false;sourceRepository:"https://github.com/Goan114/th075";adaptationNotice:"early-test"}>;
        runtimeFileLayout: "directory"; requiredShared: readonly string[]; runtimeAssets: readonly string[]; dataProvider: "retail-memory";
        package: Readonly<{dataFileId:"game-data";dataTarget:"/th075.data";maxImportBytes:number;musicSourceDirectories:Readonly<{ogg:"music"}>;musicMounts:Readonly<{ogg:"/music"}>}>;
        replay: Readonly<{prefix:"th075";extensions:readonly string[];naming:"filename"}>;
        multiplayerRuntime: "./runtime/th075/multiplayer/th075.html";
        multiplayer: Readonly<MultiplayerProductConfig>;
        features: Readonly<{thprac:false;languages:true;focusHitbox:false}>;
    }>;
    th08: Readonly<{
        thpracExtraFunctionKeys: readonly ["U"];
        cardArtwork: "th08-card.webp";
        number: "08";
        title: "東方永夜抄";
        subtitle: "Imperishable Night";
        storage: Readonly<{
            saveRoot: "/savesth08";
            scoreFile: "score.dat";
            configFiles: readonly string[];
        }>;
        runtime: "./runtime/th08/th08.html";
        musicCapabilities: Readonly<{
            midi: true;
        }>;
        musicRuntime: Readonly<{
            localOggConfigureMode: "midi-sentinel";
        }>;
        touchFire: Readonly<{
            readonly mode: "toggle";
            readonly labelKey: "touch.tapToggle";
        }>;
        support: Readonly<{
            highRefreshRate: true;
            sourceRepository: "https://github.com/YomotsuHisami/th08";
            adaptationNotice: "early-test";
        }>;
        runtimeFileLayout: "directory";
        requiredShared: readonly string[];
        runtimeAssets: readonly string[];
        dataProvider: "retail-memory";
        package: Readonly<{
            dataFileId: "game-data";
            dataTarget: "/th08.data";
            rawDataImport: Readonly<{
                fileNames: readonly string[];
            }>;
            musicSourceDirectories: Readonly<{
                ogg: "bgm-ogg";
            }>;
            musicMounts: Readonly<{
                ogg: "/bgm-ogg";
            }>;
        }>;
        replay: Readonly<{
            prefix: "th8";
        }>;
        multiplayerRuntime: "./runtime/th08/multiplayer/th08.html";
        multiplayer: Readonly<{
            titleKey: "game.title.th08mp";
            gameplay: "cooperative";
            inputTiming: Readonly<{
                rollbackLimit: 8;
                sendPredictionLimit: 8;
                measuredStartup: true;
            }>;
            playerCounts: readonly [2, 3];
            difficulties: readonly string[];
            loadouts: readonly (Readonly<{
                labelKey: "multiplayer.loadout.th08.reimuYukari";
                glyph: "霊";
                character: 0;
                shot: 0;
            }> | Readonly<{
                labelKey: "multiplayer.loadout.th08.marisaAlice";
                glyph: "魔";
                character: 1;
                shot: 0;
            }> | Readonly<{
                labelKey: "multiplayer.loadout.th08.sakuyaRemilia";
                glyph: "咲";
                character: 2;
                shot: 0;
            }> | Readonly<{
                labelKey: "multiplayer.loadout.th08.youmuYuyuko";
                glyph: "妖";
                character: 3;
                shot: 0;
            }> | Readonly<{
                labelKey: "multiplayer.loadout.th08.reimu";
                glyph: "霊";
                character: 4;
                shot: 0;
            }> | Readonly<{
                labelKey: "multiplayer.loadout.th08.yukari";
                glyph: "紫";
                character: 5;
                shot: 0;
            }> | Readonly<{
                labelKey: "multiplayer.loadout.th08.marisa";
                glyph: "魔";
                character: 6;
                shot: 0;
            }> | Readonly<{
                labelKey: "multiplayer.loadout.th08.alice";
                glyph: "愛";
                character: 7;
                shot: 0;
            }> | Readonly<{
                labelKey: "multiplayer.loadout.th08.sakuya";
                glyph: "咲";
                character: 8;
                shot: 0;
            }> | Readonly<{
                labelKey: "multiplayer.loadout.th08.remilia";
                glyph: "蕾";
                character: 9;
                shot: 0;
            }> | Readonly<{
                labelKey: "multiplayer.loadout.th08.youmu";
                glyph: "妖";
                character: 10;
                shot: 0;
            }> | Readonly<{
                labelKey: "multiplayer.loadout.th08.yuyuko";
                glyph: "幽";
                character: 11;
                shot: 0;
            }>)[];
            peerTransportGlobal: "__th08PeerTransport";
            preflightWithoutRoom: true;
            spectator: true;
            titleRoomEntry: false;
        }>;
        features: Readonly<{
            thprac: true;
            languages: true;
            focusHitbox: false;
        }>;
    }>;
    th09: Readonly<{
        cardArtwork: "th09-card.webp";
        number: "09";
        title: "東方花映塚";
        subtitle: "Phantasmagoria of Flower View";
        storage: Readonly<{
            saveRoot: "/savesth09";
            scoreFile: "score.dat";
            configFiles: readonly string[];
        }>;
        runtime: "./runtime/th09/th09.html";
        musicCapabilities: Readonly<{
            midi: false;
        }>;
        musicRuntime: Readonly<{
            localOggConfigureMode: "runtime-selection";
        }>;
        touchFire: Readonly<{
            readonly mode: "held-key";
            readonly labelKey: "touch.holdFireCharge";
            readonly key: Readonly<{
                code: "KeyZ";
                key: "z";
                keyCode: 90;
            }>;
        }>;
        support: Readonly<{
            highRefreshRate: false;
            sourceRepository: "https://github.com/YomotsuHisami/th09";
            adaptationNotice: "early-test";
        }>;
        runtimeFileLayout: "directory";
        requiredShared: readonly string[];
        runtimeAssets: readonly string[];
        dataProvider: "retail-memory";
        package: Readonly<{
            dataFileId: "game-data";
            dataTarget: "/th09.data";
            rawDataImport: Readonly<{
                fileNames: readonly string[];
            }>;
            musicSourceDirectories: Readonly<{
                ogg: "music";
            }>;
            musicMounts: Readonly<{
                ogg: "/music";
            }>;
        }>;
        replay: Readonly<{
            prefix: "th9";
        }>;
        multiplayerRuntime: "./runtime/th09/multiplayer/th09.html";
        multiplayer: Readonly<{
            titleKey: "game.title.th09mp";
            gameplay: "versus";
            inGameSelection: true;
            inputTiming: Readonly<{
                rollbackLimit: 8;
                measuredStartup: true;
                manualDelayLimit: 9;
                manualRollbackDelay: true;
            }>;
            playerCounts: readonly [2];
            difficulties: readonly string[];
            loadouts: readonly Readonly<{
                labelKey: `multiplayer.loadout.th09_${number}`;
                glyph: string;
                character: number;
                shot: 0;
            }>[];
            peerTransportGlobal: "__th09PeerTransport";
            spectator: true;
            titleRoomEntry: true;
        }>;
        features: Readonly<{
            thprac: false;
            languages: true;
            focusHitbox: false;
        }>;
    }>;
    th10: Readonly<{
        cardArtwork: "th10-card.webp";
        number: "10";
        title: "東方風神録";
        subtitle: "Mountain of Faith";
        storage: Readonly<{
            saveRoot: "/savesth10";
            scoreFile: "scoreth10.dat";
            configFiles: readonly string[];
            hintFiles: readonly string[];
        }>;
        display: Readonly<{
            faithBar: true;
        }>;
        runtime: "./runtime/th10/th10.html";
        thpracExtraFunctionKeys: readonly ["U"];
        musicCapabilities: Readonly<{
            midi: false;
        }>;
        musicRuntime: Readonly<{
            localOggConfigureMode: "midi-sentinel";
        }>;
        touchFire: Readonly<{
            readonly mode: "toggle";
            readonly labelKey: "touch.tapToggle";
        }>;
        support: Readonly<{
            highRefreshRate: true;
            sourceRepository: "https://github.com/YomotsuHisami/th10";
            adaptationNotice: "early-test";
        }>;
        runtimeFileLayout: "directory";
        requiredShared: readonly string[];
        runtimeAssets: readonly string[];
        dataProvider: "retail-memory";
        package: Readonly<{
            dataFileId: "game-data";
            dataTarget: "/th10.data";
            musicSourceDirectories: Readonly<{
                ogg: "bgm-ogg";
            }>;
            musicMounts: Readonly<{
                ogg: "/bgm-ogg";
            }>;
        }>;
        replay: Readonly<{
            prefix: "th10";
        }>;
        multiplayerRuntime: "./runtime/th10/multiplayer/th10.html";
        multiplayer: Readonly<{
            titleKey: "game.title.th10mp";
            gameplay: "cooperative";
            inputTiming: Readonly<{
                rollbackLimit: 12;
                measuredStartup: true;
            }>;
            playerCounts: readonly [2, 3];
            difficulties: readonly string[];
            loadouts: readonly (Readonly<{
                labelKey: "multiplayer.loadout.reimuA";
                glyph: "霊";
                character: 0;
                shot: 0;
            }> | Readonly<{
                labelKey: "multiplayer.loadout.reimuB";
                glyph: "霊";
                character: 0;
                shot: 1;
            }> | Readonly<{
                labelKey: "multiplayer.loadout.marisaA";
                glyph: "魔";
                character: 1;
                shot: 0;
            }> | Readonly<{
                labelKey: "multiplayer.loadout.marisaB";
                glyph: "魔";
                character: 1;
                shot: 1;
            }> | Readonly<{
                labelKey: "multiplayer.loadout.reimuC";
                glyph: "霊";
                character: 0;
                shot: 2;
            }> | Readonly<{
                labelKey: "multiplayer.loadout.marisaC";
                glyph: "魔";
                character: 1;
                shot: 2;
            }>)[];
            peerTransportGlobal: "__th10PeerTransport";
            spectator: true;
            titleRoomEntry: false;
        }>;
        features: Readonly<{
            thprac: true;
            languages: true;
            focusHitbox: false;
        }>;
    }>;
    th11: Readonly<{
        number: "11";
        title: "東方地霊殿";
        subtitle: "Subterranean Animism";
        cardArtwork: "th11-card.webp";
        storage: Readonly<{
            saveRoot: "/savesth11";
            scoreFile: "scoreth11.dat";
            configFiles: readonly string[];
        }>;
        runtime: "./runtime/th11/th11.html";
        musicCapabilities: Readonly<{
            midi: false;
        }>;
        musicRuntime: Readonly<{
            localOggConfigureMode: "runtime-selection";
        }>;
        touchFire: Readonly<{
            readonly mode: "toggle";
            readonly labelKey: "touch.tapToggle";
        }>;
        support: Readonly<{
            highRefreshRate: false;
            sourceRepository: "https://github.com/YomotsuHisami/th11";
            adaptationNotice: "early-test";
        }>;
        runtimeFileLayout: "directory";
        requiredShared: readonly string[];
        runtimeAssets: readonly string[];
        dataProvider: "retail-memory";
        package: Readonly<{
            dataFileId: "game-data";
            dataTarget: "/th11.dat";
            rawDataImport: Readonly<{
                fileNames: readonly string[];
            }>;
            musicSourceDirectories: Readonly<{
                ogg: "music";
            }>;
            musicMounts: Readonly<{
                ogg: "/music";
            }>;
        }>;
        replay: Readonly<{
            prefix: "th11";
        }>;
        multiplayerRuntime: "./runtime/th11/multiplayer/th11.html";
        multiplayer: Readonly<{
            titleKey: "game.title.th11mp";
            gameplay: "cooperative";
            inputTiming: Readonly<{
                rollbackLimit: 12;
                sendPredictionLimit: 12;
                measuredStartup: true;
                manualDelayLimit: 9;
            }>;
            playerCounts: readonly [2, 3];
            difficulties: readonly string[];
            loadouts: readonly (Readonly<{
                labelKey: "multiplayer.loadout.reimuA";
                glyph: "霊";
                character: 0;
                shot: 0;
            }> | Readonly<{
                labelKey: "multiplayer.loadout.reimuB";
                glyph: "霊";
                character: 0;
                shot: 1;
            }> | Readonly<{
                labelKey: "multiplayer.loadout.marisaA";
                glyph: "魔";
                character: 1;
                shot: 0;
            }> | Readonly<{
                labelKey: "multiplayer.loadout.marisaB";
                glyph: "魔";
                character: 1;
                shot: 1;
            }> | Readonly<{
                labelKey: "multiplayer.loadout.reimuC";
                glyph: "霊";
                character: 0;
                shot: 2;
            }> | Readonly<{
                labelKey: "multiplayer.loadout.marisaC";
                glyph: "魔";
                character: 1;
                shot: 2;
            }>)[];
            peerTransportGlobal: "__th11PeerTransport";
            spectator: true;
            titleRoomEntry: false;
        }>;
        features: Readonly<{
            thprac: true;
            languages: true;
            focusHitbox: false;
        }>;
    }>;
    th15: Readonly<{
        number: "15";
        title: "東方紺珠伝";
        subtitle: "Legacy of Lunatic Kingdom";
        cardArtwork: "th15-card.webp";
        storage: Readonly<{
            saveRoot: "/savesth15";
            scoreFile: "scoreth15.dat";
            configFiles: readonly string[];
        }>;
        runtime: "./runtime/th15/th15.html";
        musicCapabilities: Readonly<{
            midi: false;
        }>;
        musicRuntime: Readonly<{
            localOggConfigureMode: "runtime-selection";
        }>;
        touchFire: Readonly<{
            readonly mode: "toggle";
            readonly labelKey: "touch.tapToggle";
        }>;
        thpracExtraFunctionKeys: readonly ["U"];
        support: Readonly<{
            highRefreshRate: true;
            sourceRepository: "https://github.com/YomotsuHisami/th15";
            adaptationNotice: "early-test";
        }>;
        runtimeFileLayout: "directory";
        requiredShared: readonly string[];
        runtimeAssets: readonly string[];
        dataProvider: "retail-memory";
        package: Readonly<{
            dataFileId: "game-data";
            dataTarget: "/th15.dat";
            rawDataImport: Readonly<{
                fileNames: readonly string[];
            }>;
            musicSourceDirectories: Readonly<{
                ogg: "music";
            }>;
            musicMounts: Readonly<{
                ogg: "/music";
            }>;
        }>;
        replay: Readonly<{
            prefix: "th15";
        }>;
        multiplayerRuntime: "./runtime/th15/multiplayer/th15.html";
        multiplayer: Readonly<{
            titleKey: "game.title.th15mp";
            gameplay: "cooperative";
            inputTiming: Readonly<{
                rollbackLimit: 0;
                measuredStartup: true;
                manualDelayLimit: 9;
            }>;
            playerCounts: readonly [2, 3];
            difficulties: readonly string[];
            loadouts: readonly Readonly<{
                labelKey: `multiplayer.loadout.th15_${string}`;
                glyph: string;
                character: number;
                shot: 0;
            }>[];
            peerTransportGlobal: "__th15PeerTransport";
            spectator: true;
            titleRoomEntry: false;
        }>;
        features: Readonly<{
            thprac: true;
            languages: true;
            focusHitbox: false;
        }>;
    }>;
    th20: Readonly<{
        testOnly: true;
        number: "20";
        title: "東方錦上京";
        subtitle: "Fossilized Wonders";
        cardArtwork: "th20-card.webp";
        storage: Readonly<{
            saveRoot: "/savesth20";
            scoreFile: "scoreth20.dat";
            configFiles: readonly string[];
        }>;
        runtime: "./runtime/th20/th20.html";
        musicCapabilities: Readonly<{
            midi: false;
        }>;
        musicRuntime: Readonly<{
            localOggConfigureMode: "runtime-selection";
        }>;
        touchFire: Readonly<{
            readonly mode: "toggle";
            readonly labelKey: "touch.tapToggle";
        }>;
        support: Readonly<{
            highRefreshRate: false;
            sourceRepository: "https://github.com/Goan114/touhou20";
            adaptationNotice: "early-test";
            credit: Readonly<{
                name: "ぃ尐懒猫ゞ";
                url: "https://space.bilibili.com/15669619";
            }>;
        }>;
        runtimeFileLayout: "directory";
        requiredShared: readonly string[];
        runtimeAssets: readonly string[];
        dataProvider: "retail-memory";
        package: Readonly<{
            dataFileId: "game-data";
            dataTarget: "/th20.data";
            musicSourceDirectories: Readonly<{
                ogg: "bgm-ogg";
            }>;
            musicMounts: Readonly<{
                ogg: "/bgm-ogg";
            }>;
            rawDataImport: Readonly<{
                fileNames: readonly string[];
            }>;
        }>;
        replay: Readonly<{
            prefix: "th20";
        }>;
        features: Readonly<{
            thprac: false;
            languages: true;
            focusHitbox: false;
        }>;
    }>;
}>;
export type GameId = keyof typeof PRODUCT_GAMES;
export type ProductGame = (typeof PRODUCT_GAMES)[GameId];
export type MultiplayerGameId = {
    [K in GameId]: (typeof PRODUCT_GAMES)[K] extends {
        readonly multiplayerRuntime: string;
    } ? K : never;
}[GameId];
export type MultiplayerProductId = `${MultiplayerGameId}mp`;
export type ProductId = GameId | MultiplayerProductId;
export interface MultiplayerLoadoutConfig {
    labelKey: string;
    glyph: string;
    character: number;
    shot: number;
}
export interface MultiplayerInputTimingConfig {
    rollbackDefault?: boolean;
    /** Zero permits confirmed input only: no prediction, rollback or hybrid mode. */
    rollbackLimit: number;
    sendPredictionLimit?: number;
    measuredStartup?: boolean;
    manualDelayLimit?: number;
    manualRollbackDelay?: boolean;
}
export interface MultiplayerProductConfig {
    spectator?: boolean;
    titleRoomEntry?: boolean;
    loadoutFields?: readonly Readonly<{labelKey: string; dependsOn?: readonly number[]}>[];
    titleKey: string;
    gameplay: "cooperative" | "versus";
    inGameSelection?: boolean;
    inputTiming?: Readonly<MultiplayerInputTimingConfig>;
    playerCounts: readonly (2 | 3)[];
    difficulties: readonly string[];
    loadouts: readonly MultiplayerLoadoutConfig[];
    peerTransportGlobal: string;
}
/** Interpret the same product timing declaration at every shared boundary. */
export declare function multiplayerInputTimingPolicy(product: Pick<MultiplayerProductConfig, "inputTiming"> | null | undefined): {
    measuredStartup: boolean;
    rollback: boolean;
    manualRollbackDelay: boolean;
    manualDelayLimit: number;
    predictionLimit: number;
};
export declare const PRODUCT_IDS: readonly ProductId[];
export declare const DEFAULT_PRODUCT_ID: ProductId;
export declare const DEFAULT_MULTIPLAYER_PRODUCT_ID: MultiplayerProductId;
export declare function isGameId(value: string): value is GameId;
export declare function productEnabledForBuild(productId: string, testBuild?: boolean): boolean;
export declare function isProductId(value: string): value is ProductId;
export declare function isMultiplayerProductId(productId: string): productId is MultiplayerProductId;
export declare function gameIdForProduct(productId: ProductId): GameId;
export declare function gameIdForProduct(productId: string): GameId | string;
export declare function multiplayerProductIdForGame(gameId: string): MultiplayerProductId | null;
export declare function multiplayerConfigForProduct(productId: string): MultiplayerProductConfig | null;
export declare function productFeatureAvailable(gameId: string, featureId: ProductFeatureId, hostFeatures?: HostRuntimeFeatures | null): boolean;
export declare function createLocalProductManifest(): {
    protocol: string;
    shared: {
        resourceMode: "hosted";
        testBuild: boolean;
    };
    games: {
        [k: string]: {
            cardArtwork: "th06-card.webp";
            number: "06";
            title: "東方紅魔郷";
            subtitle: "the Embodiment of Scarlet Devil";
            storage: Readonly<{
                saveRoot: "/savesth06";
                scoreFile: "score.dat";
                configFiles: readonly string[];
            }>;
            runtime: "./runtime/th06/th06.html";
            musicCapabilities: Readonly<{
                midi: true;
            }>;
            musicRuntime: Readonly<{
                localOggConfigureMode: "midi-sentinel";
            }>;
            touchFire: Readonly<{
                readonly mode: "toggle";
                readonly labelKey: "touch.tapToggle";
            }>;
            support: Readonly<{
                highRefreshRate: true;
                sourceRepository: "https://github.com/YomotsuHisami/th06";
            }>;
            dataProvider: "emscripten-preload";
            package: Readonly<{
                dataFileId: "game-data";
                dataTarget: "/th06.data";
                musicSourceDirectories: Readonly<{
                    wav: "bgm";
                    ogg: "bgm";
                }>;
                musicMounts: Readonly<{
                    wav: "/bgm";
                    ogg: "/bgm";
                }>;
            }>;
            replay: Readonly<{
                prefix: "th6";
            }>;
            multiplayerRuntime: "./runtime/th06/multiplayer/th06.html";
            multiplayer: Readonly<{
                titleKey: "game.title.th06mp";
                gameplay: "cooperative";
                playerCounts: readonly [2, 3];
                difficulties: readonly string[];
                loadouts: readonly (Readonly<{
                    labelKey: "multiplayer.loadout.reimuA";
                    glyph: "霊";
                    character: 0;
                    shot: 0;
                }> | Readonly<{
                    labelKey: "multiplayer.loadout.reimuB";
                    glyph: "霊";
                    character: 0;
                    shot: 1;
                }> | Readonly<{
                    labelKey: "multiplayer.loadout.marisaA";
                    glyph: "魔";
                    character: 1;
                    shot: 0;
                }> | Readonly<{
                    labelKey: "multiplayer.loadout.marisaB";
                    glyph: "魔";
                    character: 1;
                    shot: 1;
                }>)[];
                peerTransportGlobal: "__th06PeerTransport";
                spectator: true;
                titleRoomEntry: false;
            }>;
            features: Readonly<{
                thprac: true;
                languages: true;
                focusHitbox: true;
            }>;
            music: {
                midi: {
                    files: never[];
                    supported?: undefined;
                } | {
                    files: never[];
                    supported: boolean;
                };
            };
            languageOptions: {
                id: string;
                title: string;
                pack: null;
            }[];
        } | {
            cardArtwork: "th07-card.webp";
            cardPresentation: Readonly<{
                positionPercent: 58;
                artBrightness: 0.42;
                artSaturation: 0.62;
                glowBrightness: 0.8;
                glowSaturation: 0.88;
            }>;
            number: "07";
            title: "東方妖々夢";
            subtitle: "Perfect Cherry Blossom";
            storage: Readonly<{
                saveRoot: "/savesth07";
                scoreFile: "score.dat";
                configFiles: readonly string[];
            }>;
            runtime: "./runtime/th07/th07.html";
            musicCapabilities: Readonly<{
                midi: true;
            }>;
            musicRuntime: Readonly<{
                localOggConfigureMode: "midi-sentinel";
            }>;
            touchFire: Readonly<{
                readonly mode: "toggle";
                readonly labelKey: "touch.tapToggle";
            }>;
            support: Readonly<{
                highRefreshRate: true;
                sourceRepository: "https://github.com/YomotsuHisami/th07";
            }>;
            dataProvider: "emscripten-preload";
            package: Readonly<{
                dataFileId: "game-data";
                dataTarget: "/th07.data";
                musicSourceDirectories: Readonly<{
                    wav: ".";
                    ogg: "bgm-ogg";
                }>;
                musicMounts: Readonly<{
                    wav: "/";
                    ogg: "/bgm-ogg";
                }>;
            }>;
            replay: Readonly<{
                prefix: "th7";
            }>;
            multiplayerRuntime: "./runtime/th07/multiplayer/th07.html";
            multiplayer: Readonly<{
                titleKey: "game.title.th07mp";
                gameplay: "cooperative";
                playerCounts: readonly [2, 3];
                difficulties: readonly string[];
                loadouts: readonly (Readonly<{
                    labelKey: "multiplayer.loadout.reimuA";
                    glyph: "霊";
                    character: 0;
                    shot: 0;
                }> | Readonly<{
                    labelKey: "multiplayer.loadout.reimuB";
                    glyph: "霊";
                    character: 0;
                    shot: 1;
                }> | Readonly<{
                    labelKey: "multiplayer.loadout.marisaA";
                    glyph: "魔";
                    character: 1;
                    shot: 0;
                }> | Readonly<{
                    labelKey: "multiplayer.loadout.marisaB";
                    glyph: "魔";
                    character: 1;
                    shot: 1;
                }> | Readonly<{
                    labelKey: "multiplayer.loadout.sakuyaA";
                    glyph: "咲";
                    character: 2;
                    shot: 0;
                }> | Readonly<{
                    labelKey: "multiplayer.loadout.sakuyaB";
                    glyph: "咲";
                    character: 2;
                    shot: 1;
                }>)[];
                peerTransportGlobal: "__th07PeerTransport";
                spectator: true;
                titleRoomEntry: false;
            }>;
            features: Readonly<{
                thprac: true;
                languages: true;
                focusHitbox: false;
            }>;
            music: {
                midi: {
                    files: never[];
                    supported?: undefined;
                } | {
                    files: never[];
                    supported: boolean;
                };
            };
            languageOptions: {
                id: string;
                title: string;
                pack: null;
            }[];
        } | {
            thpracExtraFunctionKeys: readonly ["U"];
            cardArtwork: "th08-card.webp";
            number: "08";
            title: "東方永夜抄";
            subtitle: "Imperishable Night";
            storage: Readonly<{
                saveRoot: "/savesth08";
                scoreFile: "score.dat";
                configFiles: readonly string[];
            }>;
            runtime: "./runtime/th08/th08.html";
            musicCapabilities: Readonly<{
                midi: true;
            }>;
            musicRuntime: Readonly<{
                localOggConfigureMode: "midi-sentinel";
            }>;
            touchFire: Readonly<{
                readonly mode: "toggle";
                readonly labelKey: "touch.tapToggle";
            }>;
            support: Readonly<{
                highRefreshRate: true;
                sourceRepository: "https://github.com/YomotsuHisami/th08";
                adaptationNotice: "early-test";
            }>;
            runtimeFileLayout: "directory";
            requiredShared: readonly string[];
            runtimeAssets: readonly string[];
            dataProvider: "retail-memory";
            package: Readonly<{
                dataFileId: "game-data";
                dataTarget: "/th08.data";
                rawDataImport: Readonly<{
                    fileNames: readonly string[];
                }>;
                musicSourceDirectories: Readonly<{
                    ogg: "bgm-ogg";
                }>;
                musicMounts: Readonly<{
                    ogg: "/bgm-ogg";
                }>;
            }>;
            replay: Readonly<{
                prefix: "th8";
            }>;
            multiplayerRuntime: "./runtime/th08/multiplayer/th08.html";
            multiplayer: Readonly<{
                titleKey: "game.title.th08mp";
                gameplay: "cooperative";
                inputTiming: Readonly<{
                    rollbackLimit: 8;
                    sendPredictionLimit: 8;
                    measuredStartup: true;
                }>;
                playerCounts: readonly [2, 3];
                difficulties: readonly string[];
                loadouts: readonly (Readonly<{
                    labelKey: "multiplayer.loadout.th08.reimuYukari";
                    glyph: "霊";
                    character: 0;
                    shot: 0;
                }> | Readonly<{
                    labelKey: "multiplayer.loadout.th08.marisaAlice";
                    glyph: "魔";
                    character: 1;
                    shot: 0;
                }> | Readonly<{
                    labelKey: "multiplayer.loadout.th08.sakuyaRemilia";
                    glyph: "咲";
                    character: 2;
                    shot: 0;
                }> | Readonly<{
                    labelKey: "multiplayer.loadout.th08.youmuYuyuko";
                    glyph: "妖";
                    character: 3;
                    shot: 0;
                }> | Readonly<{
                    labelKey: "multiplayer.loadout.th08.reimu";
                    glyph: "霊";
                    character: 4;
                    shot: 0;
                }> | Readonly<{
                    labelKey: "multiplayer.loadout.th08.yukari";
                    glyph: "紫";
                    character: 5;
                    shot: 0;
                }> | Readonly<{
                    labelKey: "multiplayer.loadout.th08.marisa";
                    glyph: "魔";
                    character: 6;
                    shot: 0;
                }> | Readonly<{
                    labelKey: "multiplayer.loadout.th08.alice";
                    glyph: "愛";
                    character: 7;
                    shot: 0;
                }> | Readonly<{
                    labelKey: "multiplayer.loadout.th08.sakuya";
                    glyph: "咲";
                    character: 8;
                    shot: 0;
                }> | Readonly<{
                    labelKey: "multiplayer.loadout.th08.remilia";
                    glyph: "蕾";
                    character: 9;
                    shot: 0;
                }> | Readonly<{
                    labelKey: "multiplayer.loadout.th08.youmu";
                    glyph: "妖";
                    character: 10;
                    shot: 0;
                }> | Readonly<{
                    labelKey: "multiplayer.loadout.th08.yuyuko";
                    glyph: "幽";
                    character: 11;
                    shot: 0;
                }>)[];
                peerTransportGlobal: "__th08PeerTransport";
                preflightWithoutRoom: true;
                spectator: true;
                titleRoomEntry: false;
            }>;
            features: Readonly<{
                thprac: true;
                languages: true;
                focusHitbox: false;
            }>;
            music: {
                midi: {
                    files: never[];
                    supported?: undefined;
                } | {
                    files: never[];
                    supported: boolean;
                };
            };
            languageOptions: {
                id: string;
                title: string;
                pack: null;
            }[];
        } | {
            cardArtwork: "th09-card.webp";
            number: "09";
            title: "東方花映塚";
            subtitle: "Phantasmagoria of Flower View";
            storage: Readonly<{
                saveRoot: "/savesth09";
                scoreFile: "score.dat";
                configFiles: readonly string[];
            }>;
            runtime: "./runtime/th09/th09.html";
            musicCapabilities: Readonly<{
                midi: false;
            }>;
            musicRuntime: Readonly<{
                localOggConfigureMode: "runtime-selection";
            }>;
            touchFire: Readonly<{
                readonly mode: "held-key";
                readonly labelKey: "touch.holdFireCharge";
                readonly key: Readonly<{
                    code: "KeyZ";
                    key: "z";
                    keyCode: 90;
                }>;
            }>;
            support: Readonly<{
                highRefreshRate: false;
                sourceRepository: "https://github.com/YomotsuHisami/th09";
                adaptationNotice: "early-test";
            }>;
            runtimeFileLayout: "directory";
            requiredShared: readonly string[];
            runtimeAssets: readonly string[];
            dataProvider: "retail-memory";
            package: Readonly<{
                dataFileId: "game-data";
                dataTarget: "/th09.data";
                rawDataImport: Readonly<{
                    fileNames: readonly string[];
                }>;
                musicSourceDirectories: Readonly<{
                    ogg: "music";
                }>;
                musicMounts: Readonly<{
                    ogg: "/music";
                }>;
            }>;
            replay: Readonly<{
                prefix: "th9";
            }>;
            multiplayerRuntime: "./runtime/th09/multiplayer/th09.html";
            multiplayer: Readonly<{
                titleKey: "game.title.th09mp";
                gameplay: "versus";
                inGameSelection: true;
                inputTiming: Readonly<{
                    rollbackLimit: 8;
                    measuredStartup: true;
                    manualDelayLimit: 9;
                    manualRollbackDelay: true;
                }>;
                playerCounts: readonly [2];
                difficulties: readonly string[];
                loadouts: readonly Readonly<{
                    labelKey: `multiplayer.loadout.th09_${number}`;
                    glyph: string;
                    character: number;
                    shot: 0;
                }>[];
                peerTransportGlobal: "__th09PeerTransport";
                spectator: true;
                titleRoomEntry: true;
            }>;
            features: Readonly<{
                thprac: false;
                languages: true;
                focusHitbox: false;
            }>;
            music: {
                midi: {
                    files: never[];
                    supported?: undefined;
                } | {
                    files: never[];
                    supported: boolean;
                };
            };
            languageOptions: {
                id: string;
                title: string;
                pack: null;
            }[];
        } | {
            cardArtwork: "th10-card.webp";
            number: "10";
            title: "東方風神録";
            subtitle: "Mountain of Faith";
            storage: Readonly<{
                saveRoot: "/savesth10";
                scoreFile: "scoreth10.dat";
                configFiles: readonly string[];
                hintFiles: readonly string[];
            }>;
            display: Readonly<{
                faithBar: true;
            }>;
            runtime: "./runtime/th10/th10.html";
            thpracExtraFunctionKeys: readonly ["U"];
            musicCapabilities: Readonly<{
                midi: false;
            }>;
            musicRuntime: Readonly<{
                localOggConfigureMode: "midi-sentinel";
            }>;
            touchFire: Readonly<{
                readonly mode: "toggle";
                readonly labelKey: "touch.tapToggle";
            }>;
            support: Readonly<{
                highRefreshRate: true;
                sourceRepository: "https://github.com/YomotsuHisami/th10";
                adaptationNotice: "early-test";
            }>;
            runtimeFileLayout: "directory";
            requiredShared: readonly string[];
            runtimeAssets: readonly string[];
            dataProvider: "retail-memory";
            package: Readonly<{
                dataFileId: "game-data";
                dataTarget: "/th10.data";
                musicSourceDirectories: Readonly<{
                    ogg: "bgm-ogg";
                }>;
                musicMounts: Readonly<{
                    ogg: "/bgm-ogg";
                }>;
            }>;
            replay: Readonly<{
                prefix: "th10";
            }>;
            multiplayerRuntime: "./runtime/th10/multiplayer/th10.html";
            multiplayer: Readonly<{
                titleKey: "game.title.th10mp";
                gameplay: "cooperative";
                inputTiming: Readonly<{
                    rollbackLimit: 12;
                    measuredStartup: true;
                }>;
                playerCounts: readonly [2, 3];
                difficulties: readonly string[];
                loadouts: readonly (Readonly<{
                    labelKey: "multiplayer.loadout.reimuA";
                    glyph: "霊";
                    character: 0;
                    shot: 0;
                }> | Readonly<{
                    labelKey: "multiplayer.loadout.reimuB";
                    glyph: "霊";
                    character: 0;
                    shot: 1;
                }> | Readonly<{
                    labelKey: "multiplayer.loadout.marisaA";
                    glyph: "魔";
                    character: 1;
                    shot: 0;
                }> | Readonly<{
                    labelKey: "multiplayer.loadout.marisaB";
                    glyph: "魔";
                    character: 1;
                    shot: 1;
                }> | Readonly<{
                    labelKey: "multiplayer.loadout.reimuC";
                    glyph: "霊";
                    character: 0;
                    shot: 2;
                }> | Readonly<{
                    labelKey: "multiplayer.loadout.marisaC";
                    glyph: "魔";
                    character: 1;
                    shot: 2;
                }>)[];
                peerTransportGlobal: "__th10PeerTransport";
                spectator: true;
                titleRoomEntry: false;
            }>;
            features: Readonly<{
                thprac: true;
                languages: true;
                focusHitbox: false;
            }>;
            music: {
                midi: {
                    files: never[];
                    supported?: undefined;
                } | {
                    files: never[];
                    supported: boolean;
                };
            };
            languageOptions: {
                id: string;
                title: string;
                pack: null;
            }[];
        } | {
            number: "11";
            title: "東方地霊殿";
            subtitle: "Subterranean Animism";
            cardArtwork: "th11-card.webp";
            storage: Readonly<{
                saveRoot: "/savesth11";
                scoreFile: "scoreth11.dat";
                configFiles: readonly string[];
            }>;
            runtime: "./runtime/th11/th11.html";
            musicCapabilities: Readonly<{
                midi: false;
            }>;
            musicRuntime: Readonly<{
                localOggConfigureMode: "runtime-selection";
            }>;
            touchFire: Readonly<{
                readonly mode: "toggle";
                readonly labelKey: "touch.tapToggle";
            }>;
            support: Readonly<{
                highRefreshRate: false;
                sourceRepository: "https://github.com/YomotsuHisami/th11";
                adaptationNotice: "early-test";
            }>;
            runtimeFileLayout: "directory";
            requiredShared: readonly string[];
            runtimeAssets: readonly string[];
            dataProvider: "retail-memory";
            package: Readonly<{
                dataFileId: "game-data";
                dataTarget: "/th11.dat";
                rawDataImport: Readonly<{
                    fileNames: readonly string[];
                }>;
                musicSourceDirectories: Readonly<{
                    ogg: "music";
                }>;
                musicMounts: Readonly<{
                    ogg: "/music";
                }>;
            }>;
            replay: Readonly<{
                prefix: "th11";
            }>;
            multiplayerRuntime: "./runtime/th11/multiplayer/th11.html";
            multiplayer: Readonly<{
                titleKey: "game.title.th11mp";
                gameplay: "cooperative";
                inputTiming: Readonly<{
                    rollbackLimit: 12;
                    sendPredictionLimit: 12;
                    measuredStartup: true;
                    manualDelayLimit: 9;
                }>;
                playerCounts: readonly [2, 3];
                difficulties: readonly string[];
                loadouts: readonly (Readonly<{
                    labelKey: "multiplayer.loadout.reimuA";
                    glyph: "霊";
                    character: 0;
                    shot: 0;
                }> | Readonly<{
                    labelKey: "multiplayer.loadout.reimuB";
                    glyph: "霊";
                    character: 0;
                    shot: 1;
                }> | Readonly<{
                    labelKey: "multiplayer.loadout.marisaA";
                    glyph: "魔";
                    character: 1;
                    shot: 0;
                }> | Readonly<{
                    labelKey: "multiplayer.loadout.marisaB";
                    glyph: "魔";
                    character: 1;
                    shot: 1;
                }> | Readonly<{
                    labelKey: "multiplayer.loadout.reimuC";
                    glyph: "霊";
                    character: 0;
                    shot: 2;
                }> | Readonly<{
                    labelKey: "multiplayer.loadout.marisaC";
                    glyph: "魔";
                    character: 1;
                    shot: 2;
                }>)[];
                peerTransportGlobal: "__th11PeerTransport";
                spectator: true;
                titleRoomEntry: false;
            }>;
            features: Readonly<{
                thprac: true;
                languages: true;
                focusHitbox: false;
            }>;
            music: {
                midi: {
                    files: never[];
                    supported?: undefined;
                } | {
                    files: never[];
                    supported: boolean;
                };
            };
            languageOptions: {
                id: string;
                title: string;
                pack: null;
            }[];
        } | {
            number: "15";
            title: "東方紺珠伝";
            subtitle: "Legacy of Lunatic Kingdom";
            cardArtwork: "th15-card.webp";
            storage: Readonly<{
                saveRoot: "/savesth15";
                scoreFile: "scoreth15.dat";
                configFiles: readonly string[];
            }>;
            runtime: "./runtime/th15/th15.html";
            musicCapabilities: Readonly<{
                midi: false;
            }>;
            musicRuntime: Readonly<{
                localOggConfigureMode: "runtime-selection";
            }>;
            touchFire: Readonly<{
                readonly mode: "toggle";
                readonly labelKey: "touch.tapToggle";
            }>;
            thpracExtraFunctionKeys: readonly ["U"];
            support: Readonly<{
                highRefreshRate: true;
                sourceRepository: "https://github.com/YomotsuHisami/th15";
                adaptationNotice: "early-test";
            }>;
            runtimeFileLayout: "directory";
            requiredShared: readonly string[];
            runtimeAssets: readonly string[];
            dataProvider: "retail-memory";
            package: Readonly<{
                dataFileId: "game-data";
                dataTarget: "/th15.dat";
                rawDataImport: Readonly<{
                    fileNames: readonly string[];
                }>;
                musicSourceDirectories: Readonly<{
                    ogg: "music";
                }>;
                musicMounts: Readonly<{
                    ogg: "/music";
                }>;
            }>;
            replay: Readonly<{
                prefix: "th15";
            }>;
            multiplayerRuntime: "./runtime/th15/multiplayer/th15.html";
            multiplayer: Readonly<{
                titleKey: "game.title.th15mp";
                gameplay: "cooperative";
                inputTiming: Readonly<{
                    rollbackLimit: 0;
                    measuredStartup: true;
                    manualDelayLimit: 9;
                }>;
                playerCounts: readonly [2, 3];
                difficulties: readonly string[];
                loadouts: readonly Readonly<{
                    labelKey: `multiplayer.loadout.th15_${string}`;
                    glyph: string;
                    character: number;
                    shot: 0;
                }>[];
                peerTransportGlobal: "__th15PeerTransport";
                spectator: true;
                titleRoomEntry: false;
            }>;
            features: Readonly<{
                thprac: true;
                languages: true;
                focusHitbox: false;
            }>;
            music: {
                midi: {
                    files: never[];
                    supported?: undefined;
                } | {
                    files: never[];
                    supported: boolean;
                };
            };
            languageOptions: {
                id: string;
                title: string;
                pack: null;
            }[];
        } | {
            testOnly: true;
            number: "20";
            title: "東方錦上京";
            subtitle: "Fossilized Wonders";
            cardArtwork: "th20-card.webp";
            storage: Readonly<{
                saveRoot: "/savesth20";
                scoreFile: "scoreth20.dat";
                configFiles: readonly string[];
            }>;
            runtime: "./runtime/th20/th20.html";
            musicCapabilities: Readonly<{
                midi: false;
            }>;
            musicRuntime: Readonly<{
                localOggConfigureMode: "runtime-selection";
            }>;
            touchFire: Readonly<{
                readonly mode: "toggle";
                readonly labelKey: "touch.tapToggle";
            }>;
            support: Readonly<{
                highRefreshRate: false;
                sourceRepository: "https://github.com/Goan114/touhou20";
                adaptationNotice: "early-test";
                credit: Readonly<{
                    name: "ぃ尐懒猫ゞ";
                    url: "https://space.bilibili.com/15669619";
                }>;
            }>;
            runtimeFileLayout: "directory";
            requiredShared: readonly string[];
            runtimeAssets: readonly string[];
            dataProvider: "retail-memory";
            package: Readonly<{
                dataFileId: "game-data";
                dataTarget: "/th20.data";
                musicSourceDirectories: Readonly<{
                    ogg: "bgm-ogg";
                }>;
                musicMounts: Readonly<{
                    ogg: "/bgm-ogg";
                }>;
                rawDataImport: Readonly<{
                    fileNames: readonly string[];
                }>;
            }>;
            replay: Readonly<{
                prefix: "th20";
            }>;
            features: Readonly<{
                thprac: false;
                languages: true;
                focusHitbox: false;
            }>;
            music: {
                midi: {
                    files: never[];
                    supported?: undefined;
                } | {
                    files: never[];
                    supported: boolean;
                };
            };
            languageOptions: {
                id: string;
                title: string;
                pack: null;
            }[];
        };
    };
};
