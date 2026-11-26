const thibpScriptRoot = '/game/script/Th_IBP/';

export function editThibpScript(fs, relative, anchor, replacement, count = 1) {
  const path = thibpScriptRoot + relative;
  const bytes = fs.readFile(path);
  const littleEndian = bytes[0] === 0xff && bytes[1] === 0xfe;
  const bigEndian = bytes[0] === 0xfe && bytes[1] === 0xff;
  const width = littleEndian || bigEndian ? 2 : 1;
  function encode(text) {
    const encoded = new Uint8Array(text.length * width);
    for (let index = 0; index < text.length; index++) {
      encoded[index * width + Number(bigEndian)] = text.charCodeAt(index);
    }
    return encoded;
  }
  const needle = encode(anchor);
  const offsets = [];
  for (let index = 0; index <= bytes.length - needle.length; index += width) {
    if (needle.every((byte, relative) => bytes[index + relative] === byte)) offsets.push(index);
  }
  if (offsets.length !== count) throw new Error('Unexpected THIBP 1.10b script: ' + relative);
  const inserted = encode(replacement);
  const next = new Uint8Array(bytes.length + count * (inserted.length - needle.length));
  let source = 0;
  let target = 0;
  for (const offset of offsets) {
    next.set(bytes.subarray(source, offset), target);
    target += offset - source;
    next.set(inserted, target);
    target += inserted.length;
    source = offset + needle.length;
  }
  next.set(bytes.subarray(source), target);
  fs.writeFile(path, next);
}

export function prepareThibpScripts(fs) {
  const cancel = 'GetVirtualKeyState( VK_CANCEL ) == KEY_PUSH';
  const anchor = [
    'if( ' + cancel + ' ){',
    '\t\t\tObjPlaySE( SE_OBJ[ SE_CANCEL ] );',
    '\t\t\tyield;',
    '\t\t\tSetAreaCommonData( COMMONAREA_SCENE, SC_DIFFICULTY, command_active );',
  ].join('\r\n');
  // Reuse the difficulty screen's cancel path and its original sound/return destination.
  editThibpScript(fs, 'scene/Scene_MainSelect.dnh', anchor,
    anchor.replace(cancel, cancel + ' || GetVirtualKeyState( VK_PAUSE ) == KEY_PUSH'));
}
