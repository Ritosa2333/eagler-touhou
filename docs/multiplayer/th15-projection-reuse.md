# TH15 projected animation matrix reuse

The quad center and its sizing reference use the same world/view/projection
matrix. Compute that product once per projected quad, preserving its original
multiplication order, explicit float rounding, reciprocal refinement and viewport
conversion. The general `project_vertex` entry retains its original contract.
Parent rotation normalization still runs before projection and remains journaled.

The implementation is identical in `ports/th15` and `ports/th15-mp`. The camera
workflow builds both production variants separately and links their actual game
objects against a frozen preceding implementation (Git blob
`feb788ccb4558a0e9b773bf0147be662807e16ae`). Each lane executes 100,000 cases and
7,400,001 checks, covering finite/non-finite projection inputs, all anchors,
culled/visible geometry, self-parent links and chains beyond the 64-step bound.
Vertices and parent post-state are compared bit for bit. These are numerical
contract tests, not full GPU/gameplay or 60 Hz acceptance.

The separate TH15 Cloud workflow retains real-archive drawing, rollback,
lifecycle, interpolation, Replay, post-window input/persistence checks and the
frozen twelve-run comparison. It does not substitute a native math benchmark
for whole-game performance or software graphics for Intel UHD hardware.
