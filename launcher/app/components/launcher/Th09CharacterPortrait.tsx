import {type CSSProperties, useState} from 'react';
import {useHref} from 'react-router';
import './th09-character-portrait.css';

// Face crop center (x, y) and side length in the 256px source portraits, by character ID.
const avatarCrops = [
  [100, 104, 132], [104, 116, 140], [125, 82, 126], [114, 107, 130],
  [111, 112, 144], [107, 110, 134], [92, 126, 130], [74, 105, 132],
  [86, 93, 132], [111, 86, 126], [150, 90, 128], [139, 89, 130],
  [131, 79, 136], [137, 76, 134], [134, 105, 132], [138, 107, 134],
];

export function Th09CharacterPortrait({character}: {character: number}) {
  const source = useHref(`/assets/th09-portraits/${character}.webp`);
  const [failed, setFailed] = useState<string>();
  if (source === failed) return null;
  return <img className="th09-character-portrait" src={source} alt="" width={256} height={256}
    loading="lazy" decoding="async" onError={() => setFailed(source)} />;
}

export function Th09CharacterAvatar({character}: {character: number}) {
  const [x, y, side] = avatarCrops[character];
  const crop = {
    '--th09-crop-size': `${256 / side * 100}%`,
    '--th09-crop-left': `${50 - x / side * 100}%`,
    '--th09-crop-top': `${50 - y / side * 100}%`,
  } as CSSProperties;
  return <span className="th09-character-avatar" style={crop} aria-hidden="true">
    <Th09CharacterPortrait character={character} />
  </span>;
}
