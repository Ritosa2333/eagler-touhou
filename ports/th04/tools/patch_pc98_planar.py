"""Equivalent planar-to-palette expansion; preserve raster, text and palette timing."""
from pathlib import Path
ROOT = Path(__file__).resolve().parent.parent
source = ROOT / 'build/native-scheduler/source/native/dosbox-x/src/hardware/vga_draw.cpp'
pinned = ROOT / 'vendor-source/core-source/dosbox-x-788fde99d3d451eab3621009186e5abe516f2b9c/src/hardware/vga_draw.cpp'
text = pinned.read_text()
table = [sum(((byte >> (7-pixel)) & 1) << (pixel*4) for pixel in range(8)) for byte in range(256)]
helper = '''
#include <emscripten.h>
static uint32_t thPlanarEnabled = 1;
extern "C" EMSCRIPTEN_KEEPALIVE uintptr_t thPlanarState() {
    return reinterpret_cast<uintptr_t>(&thPlanarEnabled);
}
static const uint32_t thPlanarSpread[256] = {
''' + ',\n'.join(','.join(f'0x{x:08x}u' for x in table[i:i+8]) for i in range(0,256,8)) + '''
};
static INLINE void TH_Planar8(uint32_t* out, uint8_t b, uint8_t r, uint8_t g, uint8_t e) {
    const uint32_t packed = thPlanarSpread[b] | (thPlanarSpread[r] << 1) |
        (thPlanarSpread[g] << 2) | (thPlanarSpread[e] << 3);
''' + '\n'.join(f'    out[{i}] = vga.dac.xlat32[(packed >> {i*4}) & 15];' for i in range(8)) + '\n}\n'
anchor = 'static uint8_t* VGA_PC98_Xlat32_Draw_Line(Bitu vidstart, Bitu line) {'
assert text.count(anchor) == 1
begin = text.index('                for (unsigned char i=0;i < 8;i++) {\n                    foreground  = (e8 & 0x80)', text.index(anchor))
end = text.index('\n\n                vidmem++;', begin)
original = text[begin:end]
replacement = '''                if (thPlanarEnabled) {
                    TH_Planar8(draw, b8, r8, g8, e8);
                    draw += 8;
                } else {
''' + original + '\n                }'
text = text[:begin] + replacement + text[end:]
text = text.replace(anchor, helper + '\n' + anchor)
source.write_text(text)
print('Patched PC-98 eight-pixel planar expansion; reference path retained for QA')
