# Recovered input controller

Independent, compiled C++ input state and controller code. Production calls
Win32, WinMM, DirectInput8W, XINPUT1_4 and WMI directly. It does not load or
execute the game EXE. `cpu_compare.cpp` is a separate original-CPU oracle with
deterministic OS input fixtures; it never samples the user's keyboard/gamepads.

## Integration

Link `th20_input` for the real input objects or `th20_input_entry_adapter` for
the recovered game globals. The latter implements entry functions 0x420f80 and
0x421040, window `sample_input()` and the `InputPrefix*&` alias to combined state.
`platform_window::InputPrefix` is now the real `input::ButtonState`; existing
0x419c00 and 0x41a280 query functions remain implemented by the window module.

Once the window, graphics/configuration and scheduler exist, call
`input::create_game_controller()`, `input::sample_game_frame()` each game update,
then `input::destroy_game_controller()`. The factory captures real window
handles and references existing configuration, log, DIDEVCAPS and scheduler
storage. No substitute graphics/window/scheduler globals are defined here.

`input::controller` is the original singleton 0x5b8898; the shutdown adapter's
`scheduler_object_005b8898` is a reference to this pointer, not a second copy.
The common 16-byte polymorphic owner is recovered in
`../runtime_core/callback_owner.hpp`. Generic retirement invokes the actual
derived C++ destructor before taking allocation lock slot 1 and freeing it.
Input sampling/reconfiguration uses the shared registry's recursive slot 15.

`button_slot(index)` reads the unique pointer array originally at 0x5b889c.
Its four pointers start null; the final loop of 0x420990 publishes the four
existing ButtonState addresses after device rebuilding. Window `input` aliases
pointer slot 2 (0x5b88a4), and menu queries use nullable slot 0. Destruction does
not clear these pointers because the original destructor does not do so.

## Evidence

| Original VA | Recovered implementation |
|---|---|
| 0x41f9b0 | Initialize every byte of 0x2c0-byte ButtonState. |
| 0x41fcb0, 0x421720 | Initialize 0x3d4-byte Device; separate reset clears only its first 16 bytes and retains input history. |
| 0x421ab0, 0x421ad0 | Keyboard/XInput device kind and logical/physical indices. |
| 0x4228b0 | Press/release edges, repeat-after-25 counters with 8/12-frame recurrence, held count and held8 mask. |
| 0x4204a0, 0x420510 | Bit-index/byte-array binding helpers, including signed negative disabled bindings and x86 shift masking. |
| 0x420760, 0x420580 | Two WinMM joystick states and four XInput startup states, quarter-range direction thresholds and combined slot 2. |
| 0x420f80, 0x421040 | WinMM probe/capabilities and graphics flag 0x1000; clear keyboard high bits through actual Get/SetKeyboardState. |
| 0x421b00 | Keyboard, DirectInput and XInput per-device sampling with original failure/history and last-device behavior. |
| 0x41fd20, 0x41fdf0, 0x421680, 0x4216a0, 0x421760, 0x421780 | Shared CallbackOwner constructor/destructor and enable/disable two scheduler nodes. |
| 0x41f8a0, 0x41fd70, 0x41f830, 0x41f7c0, 0x4217c0, 0x422b00 | Controller construction, real virtual teardown, factory/allocation and singleton lifetime. |
| 0x420990, 0x420d80 | Initialize configured mappings; rebuild devices, keyboard first then DirectInput then XInput; choose first pad when available. |
| 0x420aa0, 0x4219d0, 0x4210d0 | DirectInput device creation/cooperative levels/data formats and XInput enumeration. |
| 0x420220, 0x420180, 0x421180 | Gamepad enumeration, [-1000,+1000] axis property setup, WMI IG_/VID_/PID_ filter for XInput duplicates. |
| 0x4202c0, 0x4216e0 | Unacquire/release keyboard and pads, release DirectInput, preserve histories while resetting device headers. |
| 0x421990, 0x421950, 0x421970 | Selected device query (player 1 disabled) and per-device button state. |
| 0x41fe80 | Whole input frame: copy all four previous snapshots, poll devices, update aggregate slots, last-device kind and frame counter. |

Layout assertions retain ButtonState=0x2c0, Device=0x3d4 and Controller devices
at +0x20, bindings at +0x2e38. The original Controller is 0x2ef8 bytes. The C++
object appends one context pointer at +0x2ef8 for explicit service dependencies;
original member offsets and the actual three-entry virtual interface remain.
Graphics +0x1c..+0x47 is now named `DIDEVCAPS input_device_caps`, proved by
0x4219d0's size=44 store and GetCapabilities call.

Some behavior is deliberately unusual because the original code is unusual:

- DirectInput Poll failure always returns without updating button history,
  including after successful reacquisition. It tries Acquire once plus up to
  400 retries while the result remains DIERR_INPUTLOST. The raw 256-byte state
  was already cleared before this early return.
- XInput failure updates buttons to zero; WinMM failure preserves its old
  state. Startup aggregation can consequently retain stale WinMM bits.
- Keyboard focus loss updates buttons to zero. Keyboard sampling does not
  update last_input_kind because its test precedes building the key mask.
- Fixed keyboard shortcuts are Enter, Home/P and End/R, plus the numeric
  keypad. Escape is only the configurable fourth action's default mapping.
- XInput stick thresholds are strictly greater/less than +/-7848; triggers
  activate strictly above 29. The 12 masks match table 0x5ae220 exactly.
- The selected-player assignment in the original whole-frame function is
  subsequently overwritten by keyboard OR all gamepads. This ordering is
  retained. Player-1 selected_device always returns -1.
- Debug strings routed through 0x40c6b0 cause no log append: that original
  function is a verified five-byte return. Real 0x454150 initialization/error
  messages are preserved as the original CP932 bytes.

## Validation

```powershell
cmake -S source_reconstruction/input -B source_reconstruction/input/build -G 'Visual Studio 16 2019' -A Win32
cmake --build source_reconstruction/input/build --config Release
ctest --test-dir source_reconstruction/input/build -C Release --output-on-failure
& './source_reconstruction/input/build/Release/th20_input_cpu_compare.exe' `
  'E:\BaiduNetdiskDownload\PCGAME-Touhou.Kinjoukyou.Fossilized.Wonders-P2P\Touhou.Kinjoukyou.Fossilized.Wonders\th20.exe' `
  './source_reconstruction/input/cpu_validation.json'
```

Recorded run: **44,271 comparisons passed**, zero failures. It compares every
byte of button/device state, observed Host-call order, return values and the
whole recovered Controller payload where applicable: 10,000 state updates,
20,000 binding helpers, 500 small constructors/resets, 6,000 device polls,
768 exact key/threshold cases, 6,000 startup/probe/clear cases, 1,000 controller
frames, the mask table, controller construction and empty-COM shutdown. It
includes counter wraparound, failed polls and repeated reacquisition. Original
instructions are unchanged; only imported OS observations and an external COM
device are deterministic fixtures. Actual original recursive-lock code runs.

Source-only tests exercise 60-frame repeat timing, release/reset behavior,
focus loss, bounded reacquisition, stale legacy input, WMI product predicates,
aggregate keyboard/gamepad frames and real derived virtual destruction.
Pointer publication, initial null state and retained aliases after controller
destruction are source assertions; live device initialization is not part of
the original-CPU comparison.

## Equivalence limits and remaining integration

- Actual DirectInput/WMI discovery and COM failure cleanup are implemented,
  but not CPU-compared on live hardware. The CPU host validates game state
  given identical OS observations, not identical OS scheduling or devices.
- WMI BSTR values are explicitly VariantClear'd to avoid the original leak;
  temporary COM allocation order/leak behavior is outside the claim.
- Invalid keyboard/XInput bindings and out-of-range selected devices throw
  instead of performing original out-of-bounds accesses. The keyboard-clear
  function throws on GetKeyboardState failure rather than using indeterminate
  stack bytes. These invalid/failure domains are not claimed equivalent.
- The ControllerContext must outlive its Controller; the game adapter owns
  persistent context storage. Simultaneous reconfiguration by unsynchronized
  foreign callers and arbitrary malformed object storage are outside the
  supported domain.
- Device configuration UI, replay injection and other consumers of retained
  input fields outside the listed addresses are not implemented by this
  module. Restoring this module does not establish a playable full game or
  whole-game 1:1 behavior.
