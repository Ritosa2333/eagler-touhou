# Stage completion source

Genuine C++ for GameController stage completion (`4bc570`), clear marking (`4bd980`), stone/unlock rewards, hundredth-second playtime accounting, and non-owning Replay views. No original executable is loaded by production sources.

The local CPU oracle passed 126,976 comparisons, including complete GameController/Session/Profile bytes, returns, side effects and three full Replay accessors. UI/player/time actions during the full stage branch test are observed boundaries. The reward and playtime helper original bodies run separately in the Progress oracle: its 49,152 new checks also include profile getters; do not count this shared evidence twice.

`environment.cpp` binds the existing unique Session, SaveManager, player visibility, HUD and scene state. Pause practice/replay endpoints and achievement announcements are still explicit dependencies. Replay ownership is restored in `replay_system`; accessors do not create a second owner. This library is not a full game completion claim.
