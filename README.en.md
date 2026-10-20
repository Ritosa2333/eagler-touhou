<h1 align="center">
  <img src="launcher/docs/assets/eagler-touhou-wordmark.svg" alt="EAGLER TOUHOU" width="312">
</h1>

<div align="center">
  <p>A project to bring official Touhou Project games, fan games, community tools, and multiplayer mods to <strong>every platform</strong>, aiming to <strong>greatly lower the barrier to entry for players</strong>.</p>
  <p>
    <a href="https://qm.qq.com/q/eeUrxIltug?from=tim"><img src="https://img.shields.io/badge/QQ%20Group-1124121427-12B7F5?style=flat-square&amp;logo=tencentqq&amp;logoColor=white" alt="QQ Group 1124121427"></a>
    <img src="https://img.shields.io/badge/Chromium-%E2%89%A5108-4285F4?style=flat-square&amp;logo=googlechrome&amp;logoColor=white" alt="Chromium >=108">
    <a href="launcher/LICENSE"><img src="https://img.shields.io/badge/License-GPL--3.0--or--later-blue.svg?style=flat-square" alt="License: GPL-3.0-or-later"></a>
  </p>
  <p><a href="README.md">简体中文</a> | English</p>
</div>

> [!WARNING]
> For now, support for every platform is delivered through Web adaptations. The Web allows rapid iteration and makes games playable across all platforms.
>
> Native versions for individual platforms are planned to address performance and other concerns.

> [!TIP]
> For game support, see the [Game Restoration and Adaptation Progress](docs/TOUHOU_PROGRESS.md).<br>
> If you have started working on a game, submit an Issue or PR to update the table.<br>
> If you are interested in reverse engineering the original games or adapting official games and Touhou fangames, join our [QQ group](https://qm.qq.com/q/eeUrxIltug?from=tim). We also have a developer group.

## Overview

The project's basic goal is to let users play **on any modern device without barriers**.

While preserving the original game experience, including some original bugs:

1. Provide high refresh rates, touch controls (direct movement and menu gestures), and popular STG community tools (thcrap and thprac).
2. Provide multiplayer built primarily around predictive rollback, with input delay as a supplement and strong tolerance for high latency.
3. Provide an updatable Web App that manages touch layouts, saves, and Replays and keeps working offline.
4. Provide a multiplayer lobby where players can get into a game with just a few button presses.
5. Maintain an active development team that updates quickly and welcomes feedback and criticism.

## Features

### The Complete Original Game Experience

![The Eagler Touhou Launcher and game management interface](launcher/docs/assets/readme/launcher.webp)

- Run games in the browser with keyboard, gamepad, and touch controls. See the [game progress table](docs/TOUHOU_PROGRESS.md) for support by title.
- Import and export original-format saves and Replays. Inputs supported by the original format remain interoperable; adaptation inputs that it cannot record use extension data.
- Aims for a polished experience. It fixes numerous issues inherited from the original portable branches, including jittery bullets, uneven bullet movement, flickering, music stutters when switching tracks in single-threaded mode, and poor performance on phones.
- Supports refresh rates above 60 Hz, except for th09. \[Thanks to [reallyportable](https://github.com/some100/th07/tree/reallyportable)\]

### Input Settings

The default-expanded Input Settings group sits above Advanced Settings. The Keyboard tab provides Arrow keys and WASD presets plus named custom layouts. The Gamepad tab supports per-device button/axis learning, thresholds, and saved profiles. When enabled, Launcher gamepad mapping takes priority over in-game controller settings and requires updated Runtime packages.

The WASD layout retains original controls: arrows / WASD to move, Z / K to shoot, X / J for Bomb, Shift / L / Space to focus, and C / H for the title's existing C action. New aliases take priority when keys conflict and require updated game Runtime packages.

TH09 has a separate Local two-player editor with per-player keyboard/controller selection, saved layouts and conflict checks. Original left/right keyboard halves, WASD and arrow-key presets are available. Enabling it overrides local versus device settings without changing single-player or online mappings.

### Touch Controls

![A touch layout with adjustable positions, sizes, sensitivity, and Focus controls](launcher/docs/assets/readme/touch-controls.webp)

- Your touch path becomes the player's movement path. Available movement methods include Touch, Touch (cheat, unlimited speed), Joystick, and more. \[Thanks to [reallyportable](https://github.com/some100/th07/tree/reallyportable)\]
- Customizable touch layouts. Touch buttons can be freely moved and scaled from 60% to 300%.
- Landscape and portrait layouts can be saved separately.
- Rolling back to an older Launcher resets saved layouts that use scales above 180%.
- A Magnifier lets players zoom the game viewport at any time with a two-finger gesture. No more losing bullets because they are too small to see (x
- Touch Replays preserve the touch trajectory actually sent to the game.
- Menu gestures: swipe to move the selection cursor, tap to confirm, and tap with two fingers to go back. \[by [reallyportable](https://github.com/some100/th07/tree/reallyportable)\]
- Adjustable touch sensitivity.
- Adjustable default game viewport position.
- Multiple Focus methods, including hold, toggle, and two-finger controls.

### Popular STG Community Tools

![Using the thprac practice menu in the browser](launcher/docs/assets/readme/stg-community-tools.webp)

#### thprac

- Provides [thprac](https://github.com/touhouworldcup/thprac) integration for th06, th07, th08, and th10. th09 does not provide thprac.
- Touch devices can operate practice menus, with virtual buttons for opening the Tab Tracker and cheat menu.

#### thcrap Languages

- Provides [thcrap](https://srv.thpatch.net/) language-pack support.
- Available languages depend on the game adaptation and the packs offered by the site.
- Servers can choose which language packs to offer for each game. You can throw in a whole pile of unusual language packs; players download only the one they select.

### Multiplayer Lobby

![A multiplayer lobby with player and spectator seats](launcher/docs/assets/readme/multiplayer-lobby.webp)

- Provides th06MP, th07MP, th08MP, th09MP, and th10MP. th06MP and th07MP are based on the [th07MP rules](https://github.com/sbrik1111/th07_multi_player).
- th06MP, th07MP, th08MP, and th10MP support 2–3 players; th09MP supports 2 players.
- The lobby supports room creation, joining by room code, character and difficulty selection—including Extra and Phantasm—ready checks before the game starts, and spectators.
- Restart a run by pressing R while paused or by using the restart control.
- Multiplayer Replays can be saved.
- Uses WebRTC whenever possible, with WebSocket Relay as a fallback.
- Spectators do not occupy player seats and can only watch rather than affect game state, but they cannot join midway through a game.
- Single-player and multiplayer saves and Replays are isolated. Settings may still be shared.

### Game Packages and Offline Play

- Install game packages from the site or import a ZIP containing a legally owned copy of the game.
- Installed packages remain in the browser. Game data, OGG music, fonts, and language packs can be managed independently, while Runtime components are provided by the site.
- In secure contexts such as HTTPS or a trusted loopback address, installed games and the Launcher can run offline.
- Original game data imported by the player is never uploaded to the server; it remains in the browser.

Operators can choose among three modes based on their resources and bandwidth:

- **Hosted**: the site serves the Launcher, Runtime components, and all game resources. Players can install games directly, while the site carries all resource traffic;
- **External**: the user-facing site serves the Launcher and Runtime components, while a separate complete resource site or CDN serves large game files. Players can still install games directly without sending large-file traffic through the frontend server;
- **Import**: the site serves only the Launcher and Runtime components and does not publish original game data. Players import a ZIP made from their legally owned game copy. This suits operators without a CDN traffic budget or who do not want to host original resources.

All three modes share the same frontend, browser-local game packages, and offline-play support. See the [self-hosting guide](launcher/docs/SELF_HOSTING.md) for complete deployment instructions.

## Frequently Asked Questions

See the [FAQ](launcher/docs/FAQ.md). The website page is generated from this Markdown source. (Only Chinese is currently available.)

## License

The Launcher is licensed under the [GNU General Public License v3.0 or later](launcher/LICENSE).

Complete third-party source, asset attribution, and licensing information is available in [THIRD_PARTY.md](launcher/THIRD_PARTY.md) and [ASSETS.md](launcher/ASSETS.md).

## Star History

[![Star History Chart](https://api.star-history.com/chart?repos=yomotsuhisami/eagler-touhou&type=date&legend=top-left)](https://www.star-history.com/?repos=yomotsuhisami%2Feagler-touhou&type=date&legend=top-left)
