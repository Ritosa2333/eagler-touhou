# Historical audits

This directory preserves obsolete or superseded structural checks for provenance. Files here are **not active gates** and must not be reported as current product tests.

`obsolete-bullet-render-path-contract.mjs` predates the float/sub-pixel Draw path and intentionally remains historical because satisfying its `rintf` assertions would regress current rendering.
