# Windows D3D8 Present observer

This test-only proxy forwards `Direct3DCreate8` to the system D3D8 runtime and
records fixed-address replay state at the retail executable's `Present` boundary. It
does not write game memory, synthesize input, alter Replay/RNG/timing state, or
select a Demo or ordinary Replay. Build a game-specific 32-bit DLL with `build.ps1`; copy only the
resulting `d3d8.dll` beside an isolated oracle copy of the retail game.
If the retail D3D8 runtime cannot create a device, an unmodified
`crosire/d3d8to9` binary may be placed beside it as `d3d8to9.dll`; the observer
chains to that compatibility layer before installing the same Present hook.

The output `replay-verifier-present.bin` is an append-only stream of 64-byte
rows. Inactive rows are retained so consumers can prove Demo boundaries rather
than infer them by trimming gameplay data.

`windows-present-capture.py` checks the retail configuration before launching:
the exact size/version and option ranges must be valid, and windowed mode must
be selected. TH06's ANSI configuration filename is resolved using the Windows
code page. A missing or invalid configuration is an error, because the retail
fallback is fullscreen. Locale wrappers need a verified child code-page profile
before this launcher can support them safely.

Loading the observer DLL does not prove that Present is hooked. The injector
reports a load timeout as an error; a capture must separately observe valid,
advancing rows. TH07 late injection is not currently implemented in DllMain;
its supported observer path is the startup D3D8 proxy. Ordinary retail replay
automation and its completion checks remain unfinished. Posting keyboard
messages to a hidden window is not a verified input provider for these games.
