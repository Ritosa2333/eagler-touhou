# SoEW Native — a Windows port of 東方封魔録

Native Win32 reimplementation of *Touhou Project 2: the Story of Eastern
Wonderland* (TH02, 1997, PC-98), written from scratch in C++23 against the
reverse-engineering research of [ReC98](https://github.com/nmlgc/ReC98).

> **二次創作について / Unofficial fan project**
> 本プロジェクトは東方Projectの二次創作です。原作およびその素材の権利は
> ZUN / 上海アリス幻樂団に帰属します。本リポジトリに原作のゲームデータは含まれません。
>
> This is an unofficial fan work. Touhou Project and all of its content belong
> to ZUN / Team Shanghai Alice. **No game data is included in this repository** —
> you must own the original game and extract its assets yourself.

## Quick start (Windows x64)

**Download the source, not a game installer.** Once you have access to this
repository, use **Code → Download ZIP** or clone it. The source does not include
original game data. You need your **own Japanese TH02 `.HDI` image**. You do not
need the English patch, a PC-98 emulator, or a SoundFont for the basic FM build.

1. Install [Git for Windows](https://git-scm.com/download/win),
   [Python 3.11 or later](https://www.python.org/downloads/windows/), and
   [MSYS2](https://www.msys2.org/). In the **MSYS2 UCRT64** terminal, install the toolchain:

   ```sh
   pacman -S --needed mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-ninja
   ```

2. In **Windows PowerShell**, change to the extracted source directory and
   run the dependency setup (Git for Windows must be on `PATH`):

   ```powershell
   powershell.exe -NoProfile -ExecutionPolicy Bypass -File ./bootstrap.ps1
   ```

   Then, in **MSYS2 UCRT64**, change to the same source directory and build:

   ```sh
   cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
   cmake --build build
   ```

   Bootstrap fetches the pinned `th98port` checkout; CMake fetches pinned zlib
   if no compatible installation is available. Neither step downloads TH02 data.

3. In PowerShell, from the same source directory, replace the example path with
   the location of your image:

   ```powershell
   py -3 tools/prepare_content.py "D:/Games/TH02/japanese.hdi" build/content
   New-Item -ItemType File build/portable.ini
   .\build\SoEW.exe
   ```

   Use a new `build/content` directory: the extractor will not overwrite one.
   It reads the HDI without modifying it. `portable.ini` keeps saves and replays
   beside the EXE. If it already exists, skip the `New-Item` line.

## What works

* Stages 1–5 and Extra, with the original stage scripts, enemy dispatch and
  bullet patterns.
* Bosses: Rika, Meira, the five magic stones (including the North stone's
  defeat effect), Marisa, Mima (both forms), and Sigma.
* Practice (per-section and per-chapter), Replay recording/browsing/playback,
  Options, music room, high scores, endings.
* Japanese and English presentation. The Japanese text and the English
  translation are read from assets *you* extract from your own copy; no text is
  bundled here.
* Original-style menus: two-column Options with the original fonts and colours,
  left/right value editing.
* MIDI (TinySoundFont) or FM (98fmplayer through libth03) audio.

Smooth Background is on by default. Its terrain filter reduces shimmer during
scrolling without filtering the player, bullets, or HUD. Stage 4's terrain
motif no longer flashes between render pages, and shot-hit sparks are
half-transparent in either rendering mode.

Hold either Shift key during play to move at half speed and show Reimu's
hit point. Shift is recorded in new replays and is not needed for playback.

Practice uses the same shot selection screen as the main game. Choose Reimu's
shot type with Left/Right, then use Up/Down in the stage popup. Z or Enter
confirms; X or Esc closes the popup back to shot selection. The setup popup
shows the starting point, difficulty, stocks and seed together.

During live Story, Extra, or Practice play, Backspace opens the assist overlay. Click a row or use
F1–F7 / U: invincibility, infinite lives, lock dynamic rank, infinite bombs,
keep power on a miss, prevent boss timeouts, auto-bomb, and enemy invincibility.
Tab toggles extra game info. Tools start off and stay set across retries.
A run that uses a battle assist cannot save a replay, submit a score, or record
a clear unlock, even if the assist is turned off later. Opening the overlay or
toggling extra info does not affect any of these. Replay playback is unchanged.

This is a reimplementation, not a binary patch. The gameplay is based on the
PC-98 code, but the port has not been verified frame for frame. The Windows
scheduler, RNG initialization, and some rendering and gameplay edge cases
still differ from the original.

## Credits

SoEW Native is maintained by Dr. Hydrangea Z ([Hydrangea-Z](https://github.com/Hydrangea-Z)).

Thanks to Christian Azinn for [th98port (libth03)](https://github.com/christianazinn/th98port),
which this port uses for asset decoding and FM playback. His TH02 Practice Patch
was the reference for the chapter start points. [ReC98](https://github.com/nmlgc/ReC98)
was the main reference for the original gameplay code.
[TH05 native-practice](https://github.com/Goan114/th05/tree/native-practice)
was the reference for the stage popup layout.

## Build and test notes

The supported setup is Windows 10/11 x64 with CMake 3.25 or later, Ninja,
and MinGW-w64 GCC with C++23 support (GCC 14 or later recommended).

`bootstrap.ps1` clones [libth03](https://github.com/christianazinn/th98port) at a
pinned commit into `external/th98port` and applies the local compatibility and
audio patches in `patches/`. The upstream checkout is not vendored here. Its
license is separate from this project's MIT license; confirm redistribution
terms before sharing patched upstream code or a linked build. Pass
`-DTH98PORT_DIR=<path>` to use your own checkout.

Replay compression uses a static zlib archive (1.4 or newer). If CMake
cannot find one, it downloads, verifies and builds the pinned zlib 1.4 source
archive. No system zlib package or zlib DLL is required. To use an existing
static installation, pass `-DZLIB_ROOT=<prefix>`. To build from source offline,
verify the zlib 1.4 archive against the SHA-256 in `CMakeLists.txt`, extract it,
and pass `-DFETCHCONTENT_SOURCE_DIR_ZLIB=<source-dir>`.

Tests:

```bash
cmake -S . -B build-tests -G Ninja -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=ON
cmake --build build-tests
ctest --test-dir build-tests --output-on-failure
python -B tests/test_baseline.py
```

The default build produces only `SoEW.exe` and its build dependencies. Set
`-DSOEW_BUILD_DEV_TOOLS=ON` to build the optional probes and exporters.

Asset-dependent contracts are only registered when you pass
`-DSOEW_ASSETS=<dir>`; without it the pure-logic suites still run.

If the MinGW toolchain path contains spaces and the linker cannot find
`default-manifest.o`, copy that file from the toolchain's
`x86_64-w64-mingw32/lib/` directory to a directory whose path has no spaces,
then point both compilers at the copy:

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release \
  "-DCMAKE_CXX_FLAGS=-BC:/path/to/manifest-dir/" \
  "-DCMAKE_C_FLAGS=-BC:/path/to/manifest-dir/"
```

## Optional inputs

- **MIDI:** supply GeneralUser GS **1.44**, not 1.471, as
  `build/content/GeneralUser-GS-SoftSynth-1.44.sf2`. The audited full bank's
  SHA-256 is `00725296486f652ba42bdbf2098254762ca80239353d04b55a27454fc8edb8a8`.
  FM does not need this file. The private Practice audio checkpoint catalog
  is not included or required; without it Practice uses exact seeking.
- **Other languages:** the basic setup uses Japanese game data. The locale
  tools support specific locally supplied Japanese/English inputs checked by
  hash. Chinese story text requires a separate reviewed locale pack. None of
  those overlays is needed for the quick start or included in this repository.
- **Separate data directory:** pass `--assets "D:/path/to/content"`. Extracted
  data, personal scores, and recordings belong outside Git.

## Running

Put the assets next to the executable and create an empty marker file:

```
SoEW.exe
portable.ini        <- empty file
content/            <- the directory you built above
save/               <- created on first run
rep/                <- replays
```

`portable.ini` makes the game keep settings, scores and replays beside the EXE
instead of a per-user profile. Without it the game uses its default profile
location.

## Repository layout

```
src/             gameplay, renderers, Windows host and icon
tests/           regression suites
tools/           asset preparation and local packaging
tools/dev/       diagnostic probes and exporters
cmake/           private checkpoint fingerprints (not checkpoint data)
patches/         patches applied by bootstrap.ps1
CMakeLists.txt   build entry point
```

## Packaging a single-file build

`tools/build_standalone.py` appends a compressed asset archive to the
executable and prepends a launcher. This is meant for **private** builds:
a package built that way contains original game data and must not be published.

## License

The code in this repository is MIT (see `LICENSE`). Third-party components and
their terms are listed in `NOTICE`. The original game is not covered by any
license granted here.

## Disclaimer

Not affiliated with or endorsed by ZUN or Team Shanghai Alice. Distributed
without any warranty. If you are a rights holder and want something changed,
please open an issue.
