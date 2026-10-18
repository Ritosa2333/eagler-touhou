# Shared movement state

`motion.hpp` describes the actual 72-byte record constructed by 478530 and shared
by enemies, bombs and damage regions. `motion.cpp` restores 453e40 velocity,
453ac0 position, 4543d0 coordinate flooring, 47a1f0 combined update and 47a400
rectangle bounds. The low flag nibble selects straight, circular, elliptical
or sinusoidal motion; bit 5 suppresses updates. The unsupported mode nibbles
retain the original switch behavior, including position flooring.

`motion_cpu_validation.json` records 41,984 comparisons: 8,192 finite randomized
inputs for each of the four update/floor entry points, complete 72-byte state,
all mode/flag combinations, 8,192 bounds cases including NaNs and equality,
and 1,024 complete constructor cases. Tests use round-to-nearest and the host's
verified SSE4.1 original CRT floor path. Nonfinite trajectories and other
floating-point rounding modes are not established by this report. The ordinary
source implementation does not map or execute the original binary.

Half of the movement inputs now span approximately ±1,052, exercising the
original bounded angle-normalization loop. Enemy aggregation exposed a mode3
counterexample: the angle constructor normalizes once and the position routine
normalizes again. Both calls are preserved, including the equivalent mode4
sequence. The recorded enemy counterexample and widened oracle pass after this
correction.
