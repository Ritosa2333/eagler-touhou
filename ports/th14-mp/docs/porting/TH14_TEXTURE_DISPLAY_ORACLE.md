# TH14 texture sizes

The ANM flag at chunk byte 34 is a retail upload hint. The Japanese 1.00b
routine at 0x47cb70 uses the display scale times one half only when that
flag is set and the scale is below 2. At 1280×960 it uploads the unscaled
embedded pixels. At 960×720 it uploads at 75%; at 640×480 it uploads at
50%. Textures without the flag do not use this reduction.

The retail archive contains 433 embedded ANM texture chunks, including 256
with this flag. Among them are front/front00.png, the pause atlases, title
art and dialogue portraits. The archive also has distinct ASCII atlases for
the three modes. This rule restores the detail already in the retail data.

Run the upload oracle with the retail executable and a GLES build receipt:

    python tools/th14-texture-display-oracle.py --exe <th14.exe> --build <gles-build> --emsdk <sdk> --output <report.json>

The oracle executes retail allocation and upload requests under Unicorn and
compares allocation size and whether embedded pixels are copied unchanged
against the actual GLES build objects. The Direct3D device is a recording
boundary; this is not a GPU pixel comparison.
