import fs from 'node:fs';
import path from 'node:path';
import { fileURLToPath } from 'node:url';

const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '../..');
const musicRoom = fs.readFileSync(path.join(root, 'src/MusicRoom.cpp'), 'utf8');

function requireText(needle, label) {
  if (!musicRoom.includes(needle)) {
    throw new Error(`missing ${label}: ${needle}`);
  }
}

requireText('LoadSurface(0, "data/result/music.jpg")', 'original Music Room background');
requireText('CopySurfaceToBackBuffer(0, 0, 0, 0, 0);', 'background copy before overlays');
const pathWrite = 'arg->trackDescriptors[offset].path[charIdx] = *curChar;';
const titleWrite = 'arg->trackDescriptors[offset].title[charIdx] = *curChar;';
const pathPos = musicRoom.indexOf(pathWrite);
const titlePos = musicRoom.indexOf(titleWrite, pathPos);
const pathTerminatorPos = musicRoom.indexOf("while (*curChar == '\\n' || *curChar == '\\r')", pathPos);
const titleTerminatorPos = musicRoom.indexOf("while (*curChar == '\\n' && *curChar == '\\r')", titlePos);
if (!(pathPos >= 0 && pathTerminatorPos > pathPos && pathTerminatorPos < titlePos &&
      titlePos >= 0 && titleTerminatorPos > titlePos)) {
  throw new Error('TH07 Music Room parser drifted: path must consume CR/LF with OR, title must preserve the vanilla impossible AND');
}
const wrongAndBeforeTitle = musicRoom.indexOf("while (*curChar == '\\n' && *curChar == '\\r')", pathPos);
if (wrongAndBeforeTitle >= 0 && wrongAndBeforeTitle < titlePos) {
  throw new Error('TH07 Music Room parser regression: vanilla impossible AND was moved to the path terminator');
}
// thcrap replaces the string pointer at the formatter callsite. Translated
// UTF-8 must therefore never be materialized into vanilla's 66-byte
// TrackDescriptor fields before drawing.
requireText('MusicRoomTitleForDisplay', 'late-bound Music Room title lookup');
requireText('MusicRoomCommentForDisplay', 'late-bound Music Room comment lookup');
requireText('if (!Localization::Active() || slot == 0)', 'vanilla blank slot 0 ownership');
requireText('static_cast<std::uint16_t>(slot - 1)', 'thcrap line index to VM slot offset');
requireText('Localization::MusicComment(', 'render-time thcrap Music Room comment lookup');
requireText('std::strcmp(comment, "@")', 'line-0 numbered-title marker');
requireText('"No. %2u  "', 'numbered-title formatting contract');
requireText('formatted += MusicRoomTitleForDisplay', 'dynamic numbered-title storage');
requireText('SetAnmIdxAndExecuteScript(&arg->descriptionSprites[offset], offset + 1799)',
            'original eight text VM script slots');
if (musicRoom.includes('Localization::CopyText(descriptor.description') ||
    musicRoom.includes('std::snprintf(descriptor.description') ||
    musicRoom.includes('Localization::CopyText(descriptor.title')) {
  throw new Error('TH07 Music Room regression: translated text must not be copied into vanilla fixed-size TrackDescriptor buffers');
}

const copyPos = musicRoom.indexOf('CopySurfaceToBackBuffer(0, 0, 0, 0, 0);');
const musicVmPos = musicRoom.indexOf('DrawInterpNoRotation(&arg->vm[0]);', copyPos);
const descriptionDrawPos = musicRoom.indexOf('DrawInterpNoRotation(&arg->descriptionSprites[i]);', copyPos);
if (!(copyPos >= 0 && musicVmPos > copyPos && descriptionDrawPos > musicVmPos)) {
  throw new Error('Music Room draw ordering changed: background must precede music VM and descriptions');
}

console.log('th07 Music Room source contract: PASS');
