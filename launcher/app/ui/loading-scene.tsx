import type {ReactNode} from 'react';
import {siteBrandPath} from '../../lib/site-brand.mjs';
import {loadingVignettes} from './loading-vignettes';
import reimuPortrait from './reimu-hakurei.svg?url';
import './loading-scene.css';

const characterFaces = {
  marisa: 'face-F001',
  daiyousei: 'face-F021',
  cirno: 'face-F022',
  koakuma: 'face-F024',
  patchouli: 'face-F025',
  sakuya: 'face-F026',
  lyrica: 'face-F02C',
  merlin: 'face-F02D',
  lunasa: 'face-F02E',
  youmu: 'face-F02F',
  yuyuko: 'face-F030',
  reisen: 'face-F03C',
  eirin: 'face-F03D',
  komachi: 'face-F046',
  nitori: 'face-F04F',
  momiji: 'face-F050',
  alice: 'face-F01B',
  kogasa: 'face-F05F',
  yuuka: 'face-F016',
} as const;

type Character = 'reimu' | keyof typeof characterFaces;
type Point = readonly [x: number, y: number];
type Actor = {
  character: Character;
  at: Point;
  size?: number;
  pivot?: Point;
  action?: 'search' | 'forage' | 'ferry' | 'notice' | 'puppet' | 'startle';
};
export type LoadingSceneSpec = {label: string; cast: readonly Actor[]; artwork: ReactNode; foreground?: ReactNode};

function Mountains() {
  return <path className="loading-landscape" d="M0 154 61 120 101 137 174 95 240 134 310 104 375 141 422 117 480 148V188H0Z"/>;
}

function Mist() {
  return <path className="loading-scene-line loading-scene-mist" d="M16 155H97m22 0h49m153-2h70m16 0h52M39 171h84m247 0h78"/>;
}

function Lantern({x, y}: {x: number; y: number}) {
  return <g transform={`translate(${x} ${y})`}>
    <circle className="loading-scene-halo" data-scene-motion="breathe" cy="-9" r="23"/>
    <path className="loading-scene-ink" d="M-14-16-9-23H9l5 7ZM-11 0H11v4H2v20H-2V4h-9Z"/>
    <rect className="loading-scene-light" x="-8" y="-15" width="16" height="14" rx="2"/>
    <path className="loading-scene-line" d="M-3-15V-1m6-14V-1"/>
  </g>;
}

function Cedar({x, y, scale = 1}: {x: number; y: number; scale?: number}) {
  return <path className="loading-scene-ink" transform={`translate(${x} ${y}) scale(${scale})`}
    d="M-4 0v-19h-23l15-25h-10l14-23h-8L0-97l16 30H8l14 23H12l15 25H4V0Z"/>;
}

function Mushroom({x, y, scale = 1}: {x: number; y: number; scale?: number}) {
  return <g transform={`translate(${x} ${y}) scale(${scale})`}>
    <path className="loading-scene-moon" d="M-2-7h4L3 0h-6Z"/>
    <path className="loading-scene-petal" d="M-9-6a9 8 0 0 1 18 0Q0-3-9-6Z"/>
    <circle className="loading-scene-moon" cx="-3" cy="-10" r="1.35"/>
  </g>;
}

const scenes = {
  shrine: {
    label: '博丽神社',
    cast: [{character:'reimu', at:[219,138], action:'search'}],
    artwork: <>
      <circle className="loading-scene-halo" cx="337" cy="59" r="47"/>
      <circle className="loading-scene-moon" cx="337" cy="59" r="29"/>
      <Mountains/>
      <Mist/>
      <path className="loading-scene-ink" d="M0 180q62-34 134-5l54 11 104-3q117-24 188-3v25H0Z"/>
      <path className="loading-scene-line" d="M130 204h220m-200-10h180m-162-10h144m-126-10h108m-90-10h72"/>
      <path className="loading-scene-gate" d="M189 79h10l-6 94h-10Zm92 0h10l6 94h-10ZM177 101h126v7H177Zm59-23h8v25h-8ZM166 70q74 15 148 0l-5 11q-69 12-138 0Z"/>
      <path className="loading-scene-line" d="M199 107q41 19 82 0m-64 6-3 10 8-3-2 10m41-17 3 10-8-3 2 10"/>
      <Lantern x={78} y={179}/><Lantern x={402} y={179}/>
      <g className="loading-scene-light" data-scene-motion="drift"><circle cx="146" cy="112" r="2"/><circle cx="296" cy="150" r="1.5"/><circle cx="394" cy="124" r="2"/></g>
    </>,
    foreground: <>
      <path className="loading-scene-gate loading-scene-outline" d="M233 160h48l-4 22h-40Z"/>
      <path className="loading-scene-ink" d="m235 160 2-5h39l3 5Z"/>
      <g data-scene-motion="lid">
        <path className="loading-scene-wood loading-scene-outline" d="m233 160 4-7h39l5 7Z"/>
        <path className="loading-scene-line" d="M242 156h28"/>
      </g>
      <path className="loading-scene-line" d="M242 165l2 12m6-12 1 12m6-12v12m7-12-1 12m8-12-2 12"/>
    </>,
  },
  forest: {
    label: '魔法森林',
    cast: [{character:'marisa', at:[220,150], action:'forage'}],
    artwork: <>
      <circle className="loading-scene-halo" cx="259" cy="92" r="75"/>
      <Mountains/>
      <Mist/>
      <path className="loading-scene-ink" d="M0 182q94-29 175-6t166-4q85-21 139 5v27H0Z"/>
      <Cedar x={54} y={177} scale={1.68}/><Cedar x={98} y={172} scale={1.15}/>
      <Cedar x={146} y={174} scale={.85}/><Cedar x={341} y={171} scale={.98}/>
      <Cedar x={391} y={171} scale={1.28}/><Cedar x={442} y={180} scale={1.7}/>
      <path className="loading-scene-line loading-scene-mist" d="M123 200h37m9 0h24m85 3h38m-174-19 4-7m0 7 7-5m162 9 3-9m0 10 6-6"/>
      <g className="loading-scene-light" data-scene-motion="breathe"><circle cx="172" cy="97" r="2"/><circle cx="318" cy="118" r="1.5"/><circle cx="282" cy="73" r="2"/></g>
    </>,
    foreground: <>
      <path className="loading-scene-wood loading-scene-outline" d="M184 178h19l-3 9h-13Zm3 0q4-14 13 0Z"/>
      <Mushroom x={249} y={187} scale={.74}/><Mushroom x={278} y={184} scale={.63}/>
      <g data-scene-motion="pluck">
        <Mushroom x={257} y={177}/>
      </g>
    </>,
  },
  courtyard: {
    label: '白玉楼庭院',
    cast: [{character:'youmu', at:[179,156]}, {character:'yuyuko', at:[280,123]}],
    artwork: <>
      <circle className="loading-scene-halo" cx="313" cy="64" r="58"/>
      <circle className="loading-scene-moon" cx="313" cy="64" r="24"/>
      <Mountains/>
      <path className="loading-scene-ink" d="M216 96h130v54H216Zm-12 52h152v9H204Zm-198 36q73-23 148-3t182-14q75-7 144 15v21H6Z"/>
      <path className="loading-scene-gate" d="M196 98q24-8 41-32h90q10 17 38 32Zm27 4h5v45h-5Zm45 0h5v45h-5Zm69 0h5v45h-5Z"/>
      <path className="loading-scene-line" d="M207 101h146m-141 49h137m-133 12h124m-131 6h117m-125 6h111m-116 6h100m-261 15h87m218-1h63"/>
      <path className="loading-scene-wood" d="m85 181 2-66-24-32 6-6 24 24 21-28 7 6-19 38 6 63Z"/>
      <path className="loading-scene-gate" d="M42 88q-17-21 9-33 6-23 35-18 27-16 50 7 28 1 29 26-1 18-31 19-10 15-35 6-37 9-57-7Z"/>
      <path className="loading-scene-mist loading-scene-line" d="M50 69q27-24 48-9m1 16q17-22 47-16"/>
      <path className="loading-scene-moon" opacity=".65" data-scene-motion="drift" d="M145 143q-9-6-5-15-9 4-11 11-3 12 11 12 7 0 10-8Z"/>
      <g className="loading-scene-petal" data-scene-motion="drift">
        <ellipse cx="135" cy="117" rx="4" ry="2" transform="rotate(-27 135 117)"/>
        <ellipse cx="163" cy="101" rx="4" ry="2" transform="rotate(-22 163 101)"/>
        <ellipse cx="312" cy="124" rx="3" ry="2" transform="rotate(-22 312 124)"/>
        <ellipse cx="351" cy="145" rx="4" ry="2" transform="rotate(-30 351 145)"/>
      </g>
    </>,
  },
  lake: {
    label: '雾之湖',
    cast: [{character:'daiyousei', at:[179,128]}, {character:'cirno', at:[300,153]}],
    artwork: <>
      <circle className="loading-scene-halo" cx="240" cy="66" r="66"/>
      <circle className="loading-scene-moon" cx="240" cy="66" r="33"/>
      <Mountains/>
      <path className="loading-scene-ink" d="M0 171q35-18 86 0H0Zm480 0q-35-18-86 0h86ZM0 206q50-9 99 10H0Zm480 0q-50-9-99 10h99Z"/>
      <Mist/>
      <path className="loading-scene-line" d="M75 184h48m18 0h53m22-9h52m-75 15h99m-61 8h89m-98 8h39m64-18h73m-253 14h40M42 196h41m314 0h44"/>
      <path className="loading-scene-line loading-scene-mist" d="M13 213q13-19 4-29m10 29q0-22 12-30m-5 31q11-14 17-13m416 12q-13-19-4-29m-10 29q0-22-12-30m5 31q-11-14-17-13"/>
      <path className="loading-scene-floe loading-scene-outline" d="m273 174 30-6 27 6 10 8-20 5-40-2-17-5Z"/>
      <path className="loading-scene-line" d="m287 180 15-4 14 2 7-2"/>
      <path className="loading-scene-floe" data-scene-motion="drift" d="m326 145 4-8 4 8-4 5Zm15 10 3-5 3 5-3 4Zm-1-22 2-4 2 4-2 3Z"/>
      <g className="loading-scene-light" data-scene-motion="breathe"><circle cx="62" cy="145" r="2.5"/><circle cx="156" cy="114" r="2"/><circle cx="315" cy="127" r="2"/><circle cx="427" cy="154" r="2.5"/></g>
      <g className="loading-scene-light loading-scene-late" data-scene-motion="drift"><circle cx="121" cy="98" r="1.5"/><circle cx="350" cy="93" r="1.5"/><circle cx="384" cy="130" r="2"/></g>
    </>,
  },
  ...loadingVignettes,
} satisfies Record<string, LoadingSceneSpec>;

export type LoadingSceneId = keyof typeof scenes;
export const loadingSceneOptions = Object.entries(scenes).map(([id, {label}]) => ({id: id as LoadingSceneId, label}));

export function LoadingScene({scene, assetUrl}: {scene: LoadingSceneId; assetUrl(path: string): string}) {
  const {cast, artwork, foreground}: LoadingSceneSpec = scenes[scene];
  return <div className="loading-scene" data-loading-scene={scene} aria-hidden="true">
    <svg className="loading-scene-art" viewBox="0 0 480 226" aria-hidden="true" focusable="false">{artwork}</svg>
    {cast.map(({character, at:[x,y], size = 48, pivot, action}) => <img key={`${character}:${x}:${y}`} className="loading-scene-portrait"
      data-character={character} data-scene-motion={action} alt="" draggable={false}
      src={character === 'reimu' ? reimuPortrait : assetUrl(siteBrandPath(characterFaces[character]))}
      style={{
        left: `${x / 4.8}%`, top: `${y / 2.26}%`, inlineSize: `${size / 4.8}%`,
        transformOrigin: pivot && `${50 + (pivot[0] - x) / size * 100}% ${50 + (pivot[1] - y) / size * 100}%`,
      }}/>)}
    {foreground && <svg className="loading-scene-art loading-scene-foreground" viewBox="0 0 480 226"
      aria-hidden="true" focusable="false">{foreground}</svg>}
  </div>;
}
