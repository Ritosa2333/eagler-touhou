# thcrap localization porting notes

This document records the low-level lessons from adapting the thcrap TH06
Simplified Chinese patch to the portable game. It is primarily a handoff for
the next game port: read the target game's thcrap source and reproduce its
contracts; do not treat this TH06 implementation as a patch template for TH07.

The integration is experimental and is compiled only with
`TH_ENABLE_THCRAP=ON`. Translation disabled, or a build made with the default
`OFF` value, must remain the Japanese-game regression baseline.

## Source of truth

The visible output of an original executable patched by thcrap is the
reference. The important implementation sources used for TH06 were:

- `thcrap_tsa/src/textimage.cpp`: text-image texture loading, sprite
  replacement, atlas slicing and optional replacement scripts;
- `thcrap_tsa/src/layout.cpp` and its TH06 font-cache path: the intercepted
  `CreateFont` parameters, GDI font creation and measurement behavior;
- `thcrap-tsa/base_tsa/th06.js` and `th06.v1.02h.js`: TH06-specific
  breakpoints, binhacks, sprite dimensions, image slots and addresses;
- `thcrap-tsa/base_tsa/th06/text.anm`: the widened text sprites and their real
  ANM timing/position scripts;
- the selected language repository's `th06` directory: prepared strings,
  message files and `ti_*.png` atlases.

In this workspace the upstream trees are normally next to the game repository
under `../dependencies/upstream-thcrap` and
`../dependencies/upstream-thcrap-tsa`. They are research inputs, not runtime
dependencies. Use `thanm -l` from `../dependencies/thtk-bin-12` to inspect ANM
scripts instead of guessing their positions or animation timing.

thcrap changes both data and executable behavior. Copying downloaded strings
and PNG files alone cannot reproduce the result. Every relevant `breakpoint`,
`binhack`, replacement sprite and replacement script must be classified as
one of these portable responsibilities:

1. resource preparation or lookup;
2. game state/channel selection;
3. text rasterization and measurement;
4. sprite/ANM layout and animation;
5. renderer texture, alpha and blend semantics.

## Runtime boundary

The current port consumes an offline, deterministic runtime override rather
than contacting thcrap servers while the game is running. `Localization.cpp`
reads prepared options and lookup tables, while normal game code asks the
small `Localization` interface for a translated value or image. Missing or
invalid localization data must fall back to the original game data.

Keep these boundaries when adding another language or game:

- downloading, patch-stack resolution, validation and caching belong to a
  preparation tool or launcher;
- the game consumes a versioned local resource pack only;
- copyrighted original DAT contents and generated game/build artifacts must
  never be added to the repository;
- translation must be selectable per game/version and must not silently use a
  resource made for a different executable or message format;
- do not turn translation into global string replacement. Music Room, stage
  title, in-game music title, dialogue, spell names and result screens are
  distinct channels with different storage and rendering rules.

## Text rasterization: reproduce GDI, not merely the font file

This was the most persistent source of output that looked close but was not
faithful. TH06 thcrap rasterizes game text through Windows GDI. SDL_ttf using
the same nominal font still differs in glyph weight, advance width, baseline,
antialiasing and shadow coverage.

The faithful Windows path in `TextHelper.cpp` therefore reproduces the
observed thcrap contract:

- `MS Gothic` selected through GDI;
- requested height is twice the game's logical `fontHeight`;
- `FW_BOLD` (weight 700), `SHIFTJIS_CHARSET` and
  `ANTIALIASED_QUALITY`;
- rasterize at 2x resolution, then copy the full 32-pixel-high source region
  into the logical 16-pixel text sprite;
- draw the shadow and main glyph at thcrap's actual offsets, not an invented
  outline effect;
- use the same GDI font for measurement and drawing. Mixing GDI rendering with
  SDL_ttf measurement shifts centered and right-aligned strings.

The original TH06 text path creates an A1 surface, pre-fills its alpha bit,
lets GDI alter it, then inverts it. Black shadow pixels therefore cannot be
identified by RGB alone. The portable 32-bit DIB path derives coverage from
the DIB high byte so black shadows survive. The original post-copy gradient
and alpha behavior also remain part of the output contract.

Non-Windows targets currently use an SDL_ttf fallback. It is functional, not
proof of pixel equivalence to thcrap/GDI. Do not claim Web or mobile visual
acceptance from the Windows result.

## Shift-JIS, UTF-8 and width are separate problems

Original TH06 assets use Shift-JIS and byte-oriented fixed buffers. Prepared
translations use UTF-8. Detect/convert the encoding at the rasterization
boundary, but preserve original strings unchanged when localization is off.

Never use a byte count as a display width for UTF-8. In particular:

- centered and right-aligned text uses the width measured by the active
  rasterizer;
- Music Room wrapping/splitting uses Unicode code points or display columns,
  not the vanilla 32-byte split;
- copying into game records must be bounded and must not cut a UTF-8 sequence;
- format strings from data are never passed as the C format argument. Use a
  fixed `"%s"` wrapper.

TH06 base_tsa also changes the text-rendering functions to use the active
sprite's width and height rather than its backing texture dimensions. This is
essential for widened text sprites stored in a larger shared surface. The
portable equivalents live in `AnmManager` text drawing/alignment paths. Using
the texture dimensions caused clipping, incorrect centering and dialogue text
starting outside its box.

The localized intermediate text surface is wider than the vanilla 640-pixel
surface (currently 1024 pixels). Enlarging only the final sprite is
insufficient: the temporary raster surface and copy rectangle must also fit
the widened text.

## Text-image atlases and TH06 sprite matrices

Stage titles, in-game music titles and boss title/name are supplied by thcrap
as authored RGBA atlases. For TH06, base_tsa uses texture slots 47--50 and
declares logical slices independently of the complete atlas dimensions:

| Channel | Image | Logical slice |
| --- | --- | --- |
| Stage title | `ti_sttitle.png` | 384 x 16 |
| In-game music title | `ti_bgm.png` | 384 x 32 |
| Boss title | `ti_bosstitle.png` | 384 x 64 |
| Boss name | `ti_bossname.png` | 384 x 64 |

Three details were required to avoid distortion or missing text:

1. thcrap passes color key `0` to `D3DXCreateTextureFromFileInMemoryEx`.
   Opaque and semi-transparent black pixels in these PNGs are authored
   shadows/outlines. Passing the port's `COLOR_BLACK` value erased them.
2. Fully transparent pixels may contain arbitrary RGB. Clear RGB only where
   alpha is zero before linear sampling, otherwise their color leaks into the
   edge as a white fringe. Do not apply this cleanup globally to original
   textures.
3. TH06 `Draw2` transforms a fixed 256 x 256 quad. A thcrap text-image sprite
   uses its declared logical slice size, not `slice / atlas size`, for the VM
   matrix. Deriving the matrix from a 384 x 112 atlas turned a 384 x 16 row
   into a compressed and vertically stretched image.

The replacement ANM script is part of the resource contract too. The in-game
music title uses the byte-equivalent base_tsa script that slides the 384 x 32
image in, holds it, and exits. Replacing only the texture while retaining an
incompatible original script made the title appear offscreen or disappear.

Boss title and boss name share the thcrap canvas position and have an explicit
layer order. Do not retain the two vanilla line origins after switching both
VMs to 384 x 64 canvases, and do not assume their draw order is cosmetic.

## Dialogue, spell cards and bombs

These channels exposed layout assumptions outside the string renderer.

### Dialogue

base_tsa widens dialogue text sprite `0x702` from the vanilla width to 320
pixels. It also patches both edges of the translucent dialogue box to use the
end X of that sprite. The correct portable behavior is therefore derived:

- center the widened sprite using the original ANM script;
- derive the box width from the active dialogue sprite;
- retain the 16-pixel inset between box and text;
- render the two dialogue lines through the same measured GDI path;
- keep portrait, boss-introduction layers and message opcode timing in the
  original game flow.

Moving the text by an arbitrary screenshot offset fixed one line while
breaking another. The real fix was to carry over the widened-sprite and
dialog-box contracts together.

### Enemy spell cards and player bombs

Enemy spell cards and player bombs are separate call paths and animations.
Fixing the enemy name does not fix the player bomb. For both, the translated
measured width must feed every consumer that base_tsa patches: text surface
width, sprite width, anchor/alignment and the associated red/background bar.
Leaving any consumer on the original byte-derived width produced bars that
started offscreen, text that disappeared, or content that was displaced from
the bar.

Do not collapse these paths into a generic overlay. Preserve the original
trigger, portrait/background ANM, lifetime and replay behavior, and replace
only the values thcrap replaces.

## Music Room

Music Room translation is not a single title lookup. It includes the track
title, numbered fallback line and multiple description lines. The original
code uses two sprites per line and splits at byte 32; that is invalid for
UTF-8. The localized path renders a wide logical line and splits/copies by
code point or display column. It must rerender the description when selection
changes and must retain the original menu state/input/audio behavior.

## Debugging and verification

Developer tools are guarded by `TH_DEV_TOOLS` and are `OFF` by default. They
may be retained because they make deterministic visual reproduction cheaper:

- `--music-room` opens the real Music Room transition;
- `--stage1-text`, `--stage1-spell`, `--stage1-bomb` and
  `--stage1-dialogue` exercise individual real game channels;
- `F5` cycles developer logic speed 1x, 4x and 8x for replay inspection.

These are test entry points, not alternate production implementations. A
channel is accepted only after its normal game trigger is also exercised.

For visual comparison:

1. use the same original game version, thcrap patch stack, language pack,
   configuration, DPI and logical replay frame;
2. capture each client area independently; overlapping desktop windows are
   not reliable crops;
3. compare position, bounds, glyph weight, shadow, alpha edge and lifetime;
4. compare translation disabled against the Japanese baseline after every
   renderer/text change;
5. distinguish build success, desktop process startup, desktop visual
   acceptance, Web runtime and mobile runtime in the report.

The TH06 desktop result was visually accepted for Music Room, Stage 1 title,
in-game music title, spell names, player bomb, boss title/name and dialogue.
An accepted side-by-side capture still showed small renderer-side differences;
their exact D3D8/GLES sampling or blending cause was not closed because the
remaining difference was accepted. Treat that as an open comparison boundary,
not as proof of pixel-identical rendering or as a confirmed root cause.

Recommended build matrix for regression work:

- localization `ON`, developer tools `ON`: targeted harness and replay work;
- localization `ON`, developer tools `OFF`: release-shape localized build;
- localization `OFF`, developer tools `OFF`: Japanese default regression.

## Guidance for TH07 and later games

Do not transplant the TH06 renderer or layout fixes mechanically. Start from
the target game's own executable behavior, base_tsa files, ANM assets and
`textimage.cpp` structures.

In particular:

- TH06's portable renderer keeps its RGBA vertex-color/modulation contract;
  this is not evidence that TH07 has the same color layout or swizzle;
- sprite structure sizes, register-order fields, text-image slots, scripts,
  font hooks and message formats differ by game;
- inspect every target-game base_tsa breakpoint/binhack and map it to a
  portable responsibility before implementing;
- establish a desktop original-thcrap reference first, then port the verified
  deterministic resources and behavior to Web;
- keep translation disabled as a strict original-language regression test;
- when a generalized renderer change has side effects, revert it and repair
  only the demonstrated channel from the source-level contract.

The reusable architecture is the separation of preparation, lookup,
rasterization, layout and rendering. The numeric constants and rendering
semantics remain game-specific.
