# TH11 C function-key adaptation

Upstream-tracking `main`: 7a315bc. Canonical Eagler base: 2506ac9.
Experiment: `experiment/th11-function-key-20261004`, `_scratch/th11-function-key`.
The experiment commit is promoted by fast-forward only after review and the
browser gates. No generated runtime, private assets or resource pack is committed.

KeyC adds logical bit 4. Reimu A uses its pressed edge at an original horizontal
boundary to enter the existing second-tap warp state, retaining enemy/Bomb gates,
sounds, options and crossing behavior. The dedicated shortcut allows persistent
mobile Shot/Focus; the original double-tap branch is otherwise unchanged. Marisa
B accepts the edge alongside original bit 0x400 in its existing formation branch.
Other shot types do not gain these actions. Existing Replay input records bit 4.

Passing experiment gates: production build/package (diagnostic exports excluded),
browser C sampling, both gap boundaries, center rejection, Marisa B one switch
while C is held and a second switch on a second press, C formation switches saved
and reproduced from Replay, other-shot rejection, and existing THPrac regression
across Chinese/English/Japanese and portrait/landscape/tablet input viewports.

Repeat build/package and browser gates in canonical Eagler after promotion.
Physical Android/iPad tests and the extended native keyboard oracle were not run;
they must not be relabeled as PASS. C shortcuts require the new runtime when
replaying; this does not attest compatibility with older runtimes.
