// Windows exports three 16-row masks per codepoint: Japanese, Hans and Hant.
const faceBytes = 33, recordBytes = 4 + 3 * faceBytes;

export function glyphCells(buffer) {
    const data = new DataView(buffer);
    if (new TextDecoder().decode(buffer.slice(0,8)) !== 'SOEWGDF1' || (data.byteLength-8) % recordBytes)
        throw Error('Invalid SoEW glyph atlas');
    const positions = new Map();
    for (let offset = 8; offset < data.byteLength; offset += recordBytes)
        positions.set(data.getUint32(offset,true),offset+4);
    const location = (cp,face) => (positions.get(cp) ?? positions.get(0xfffd)) + face*faceBytes;
    return {
        has(cp,face) { return positions.has(cp) && data.getUint8(location(cp,face)) !== 0; },
        paint(pixels,scalars,w,h,x,y,color,face) {
            for (const cp of scalars) {
                const width = cp < 128 || (cp >= 0xff61 && cp <= 0xff9f) ? 8 : 16;
                if (x >= w) break;
                const offset = location(cp,face)+1;
                for (let row = Math.max(0,-y); row < Math.min(16,h-y); row++) {
                    const mask = data.getUint16(offset+row*2,true);
                    for (let col = Math.max(0,-x); mask && col < Math.min(width,w-x); col++)
                        if (mask & (1 << col)) pixels[(y+row)*w+x+col] = color;
                }
                x += width;
            }
        },
    };
}
