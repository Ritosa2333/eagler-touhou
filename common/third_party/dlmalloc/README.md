# Simulation allocator

`dlmalloc.inc` is Doug Lea's public-domain dlmalloc 2.8.6, with Emscripten's compatibility header, retained from [apeirs/th11 at 324fb143](https://github.com/apeirs/th11/blob/324fb143e78560ae34f5930a19048e34035eb2f8/th11_web/cpp/multiplayer/dlmalloc_sim.inc). Its copyright dedication and upstream notes are included in the source.

`src/netplay/SimulationHeap.cpp` builds a separate fixed-base mspace with operating-system growth disabled. Only explicitly linked write-journal products use this allocator.
