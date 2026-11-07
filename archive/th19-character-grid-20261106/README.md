# Character-grid entrance

The original title.anm grid roots76 (Story) and77 (VS) create character children27..45. Native controller0x556340 creates the root and heading209, creates cursors at timer15, and enables selection/highlight after timer30. Rendering mode8 uses the rotated sprite plane in0x4c6910.

The browser uses these original ANM scripts instead of a hand-written scale/alpha approximation. Icon scripts wait by row (0..18 updates), use negative X scale and Y rotation from -pi/2 toward pi over30 updates, restore ordinary rendering/positive scale, and dim RGB to64. Character selection invokes the original character-specific highlight/dehighlight labels. Locked slots use interrupt2, preserving the ongoing entry sequence. Exit interrupt1 keeps the script-driven15-update rotation. Heading209 also replaces the hand-written16-update slide with its original20-update interpolation.

The title sprite renderer projects the Y-rotated icon plane, retaining the sign of scale. Parent translation and per-icon original coordinates determine layout. The icon path uses Y rotation only; this change does not claim full 3D renderer equivalence for arbitrary animations.

Native C++ regression executes the extracted original ANM for both roots, checking19/20 children, delayed final row, mirrored intermediate plane, settled color/scale, highlight and exit retirement. Browser regression captures both modes at seven phases and validates actual VM state. Menu return regression is repeated after the change.

Sources: [original script dump](th19-hex-scripts.txt), [EXE renderer/lookup](th19-hex-native.c). Original EXE SHA256: `8918fb39d511c9906ef769c6dd2761e7cb44ded5b7c4c1cd99c8992d12c078e9`. Controller decompilation is archived in ../th19-menu-20261106/th19-menu-helpers-20261106.c. This is an EXE/asset-based comparison; no retail-versus-browser synchronized video comparison is claimed.

Validated build WASM: `18d5438a1dfc4f1d9f078c3255863ded4d0f3481a9fa38caf994d09f888aa1db`. [Browser phases](browser-report.json).
