# NP21 runtime

Source: https://github.com/irori/np2-wasm
License: BSD-3-Clause, see LICENSE.
Copied from the local touhou-np2 installation. Native JS/WASM and font.bmp are unchanged.
font_cn.bmp is from the user-provided game bundle.
The local wrapper delegates visibility handling to app.js and propagates import errors.
Local multiplayer loads np21-60.js; original solo loads np21-solo.js, preserving
the stock scheduler and sound. Generated solo/local/lockstep adapters use
TH04's audited static WASM function-table cache; upstream np21.js/np21.wasm stay unchanged.
SHA256SUMS.json records the packaged files.
