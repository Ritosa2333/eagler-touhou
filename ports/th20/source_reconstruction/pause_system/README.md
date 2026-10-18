# PauseInf source

Actual 0x2d8-byte PauseInf with two owning Cursor containers, 25 Replay metadata owners, scheduler lifetime, four pause/result entrances, full menu state transitions, continue state restoration, high-score entry and Replay saving. All production operations call reconstructed C++ and normal OS/DirectX APIs.

The isolated original CPU oracle passed 114,688 comparisons of the four pause/result entrances, two resume routines and main update gate, including full PauseInf bytes, Game flags, input latch, clock scale, Replay finish flag and ordered boundary traces. Audio/ANM/menu work is observed at explicit test boundaries; that evidence does not validate the complete UI. Whole menu/history/constructor/capture comparisons remain in progress. Draw validation is performed separately in the shared Sprite oracle.

The HelpInf (`4bfc70`, global `5c4d24`) and OptionInf (`4e1080`, global `5c60b8`) remain named source dependencies while their modules are recovered. Replay ownership belongs to `replay_system`; no second Replay is allocated here. No executable machine code is used in production.
