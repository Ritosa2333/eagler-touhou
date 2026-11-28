import {useState} from 'react';
import {useHref} from 'react-router';

export function Th09CharacterPortrait({character}: {character: number}) {
  const directory = import.meta.env?.DEV ? '/.cache/th09-portraits' : '/assets/th09-portraits';
  const source = useHref(`${directory}/${character}.webp`);
  const [failed, setFailed] = useState<string>();
  if (source === failed) return null;
  return <img className="th09-character-portrait" src={source} alt="" width={256} height={256}
    loading="lazy" decoding="async" onError={() => setFailed(source)} />;
}
