# Player callback registration and return validation

The current `cpu_validation.json` passes 1,032,161 checks with no failures.
Its build-time hashes include `initialize.cpp`, `frame.cpp` and the respective
CPU fixtures. This does not certify complete gameplay equivalence.

`4f9520` uses disabled registration wrappers `412310` and `4123b0` for update
priority 29 (`4feda0`) and draw priority 30 (`4fefb0`). The test executes both
original wrappers, including original userdata setup and `4127f0` disabling.
Only node allocation and intrusive insertion are recorded endpoints. The test
checks callback identity, priority, userdata and the disabled flag, then checks
four successful source Player factories against those actual native flags.
The source factory leaves both callbacks disabled for the loading process to
enable after dependency initialization.

The 5,600 complete main-frame cases now compare `EAX` as well as state and
effect traces. `4f7430` returns 1, the scheduler's keep-callback result; returning
0 would retire the node even when all Player object bytes matched. These cases
raise the main-frame group from 28,000 to 33,600 checks. Existing recorded
resource/device boundaries and active-shot limitations remain as documented
in `README.md`.
