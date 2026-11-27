import type {CSSProperties, ReactNode} from 'react';
import type {LoadingSceneSpec} from './loading-scene';
import './loading-vignettes.css';

function Cue({x, y, pose, dim, rest, delay, children}: {
  x: number; y: number; pose: string; dim?: number; rest?: number; delay?: number; children: ReactNode;
}) {
  return <g transform={`translate(${x} ${y})`}>
    <g data-scene-motion="cue" style={{
      '--v-cue-pose': pose, '--v-cue-opacity': dim, '--v-cue-rest': rest,
      animationDelay: delay == null ? undefined : `${delay}ms`,
    } as CSSProperties}>{children}</g>
  </g>;
}

function Bookcase({x, y}: {x: number; y: number}) {
  return <g transform={`translate(${x} ${y})`}>
    <rect className="v-shadow" width="72" height="106" rx="2"/>
    <path className="v-accent" d="M8 15h8v26H8zm25 6h7v20h-7zm21-10h7v30h-7zM19 57h10v26H19zm20 3h8v23h-8zm16-4h8v27h-8z"/>
    <path className="v-gold" d="M18 23h10v18H18zm25-7h8v25h-8zM9 64h7v19H9zm22-10h6v29h-6zm18 15h5v14h-5z"/>
    <path className="v-line" d="M0 43h72M0 85h72M3 102V5h66v97"/>
  </g>;
}

function Sunflower({x, y, size, delay}: {x: number; y: number; size: number; delay: number}) {
  return <g transform={`translate(${x} ${y}) scale(${size})`}>
    <Cue x={0} y={0} pose="skewX(-5deg)" delay={delay}>
      <path className="v-stem" d="M0 0V-35"/>
      <path className="v-green" d="M0-12q-15-13-22-9 6 10 22 9Zm0-7q8-15 19-14-3 9-19 14Z"/>
      <path className="v-gold" d="M0-59q7-6 10 2 9-2 10 6 9 3 4 10 5 7-4 10-1 8-10 6-3 8-10 2-7 6-10-2-9 2-10-6-9-3-4-10-5-7 4-10 1-8 10-6 3-8 10-2Z"/>
      <circle className="v-shadow" cx="0" cy="-41" r="10"/>
      <path className="v-line" d="M-4-43h1m5-2h1m-1 7h1m-6 0h1"/>
    </Cue>
  </g>;
}

function SpiderLily({x, y, size}: {x: number; y: number; size: number}) {
  return <path className="v-lily" transform={`translate(${x} ${y}) scale(${size})`}
    d="M0 0v-16m0 0q-7-8-12-8m12 8q-3-14-6-15m6 15q3-14 6-15M0-16q7-8 12-8m-12 8q-9-3-12 1m12-1q9-3 12 1"/>;
}

const wallDolls = [87, 128, 353, 394].map(x => ({
  character:'alice' as const, at:[x, 106] as const, size:32,
}));
const workDoll = {character:'alice', at:[271,121], size:34, pivot:[271,75], action:'puppet'} as const;

export const loadingVignettes = {
  'mansion-clock': {
    label: '红魔馆 · 咲夜停表',
    cast: [{character:'sakuya', at:[197,119]}],
    artwork: <>
      <path className="v-wall" d="M33 167V90l54-52h306l54 52v77Z"/>
      <path className="v-line" d="M37 99l56-48h294l56 48M110 50v113m260-113v113M30 159h420m-423 7h426"/>
      <path className="v-shadow" d="M127 142V89a34 34 0 0 1 68 0v53Zm158 0V89a34 34 0 0 1 68 0v53Z"/>
      <path className="v-accent" d="M134 134V89a27 27 0 0 1 54 0v45Zm158 0V89a27 27 0 0 1 54 0v45Z"/>
      <path className="v-line" d="M161 64v70m-27-44h54m-54 23h54m131-49v70m-27-44h54m-54 23h54"/>
      <path className="v-shadow" d="M23 164h434l23 37H0Z"/>
      <path className="v-line" d="M23 201h434m-320 0 40-35m166 35-40-35"/>
      <path className="v-wood" d="M222 151h92v8h-92Zm7 8h5v29h-5Zm73 0h5v29h-5Z"/>
      <g transform="translate(241 126)">
        <circle className="v-gold" cy="-15" r="4"/>
        <circle className="v-gold v-outline" r="15"/>
        <circle className="v-paper" r="11"/>
        <path className="v-line" d="M0-8v2m8 6H6m-6 8V6m-8-6h2"/>
        <path className="v-hand" data-scene-motion="time-hand" d="M0 0v-7"/>
      </g>
      <path className="v-paper v-outline" d="M278 130h23q-2 16-11 16t-12-16Zm23 4h4q0 7-7 7m-25 8h33"/>
      <path className="v-gold" data-scene-motion="time-drop" d="M289 111q-6 7 0 8 6-1 0-8Z"/>
    </>,
  },
  library: {
    label: '红魔馆 · 图书馆',
    cast: [{character:'patchouli', at:[213,119]}, {character:'koakuma', at:[337,112]}],
    artwork: <>
      <path className="v-wall" d="M24 164V56q80-22 216-22t216 22v108Z"/>
      <path className="v-shadow" d="M0 174h480v29H0Z"/>
      <Bookcase x={63} y={52}/><Bookcase x={366} y={52}/>
      <path className="v-line" d="M149 150V82q7-26 37-34h107q39 9 46 34v19m-190 56h207M51 195h378"/>
      <path className="v-wood" d="M159 151h157l-7 10h-145Zm10 10h7v31h-7Zm132 0h7v31h-7Z"/>
      <path className="v-gold" d="M286 151h19l-7-4v-22h-4v22Zm2-33h15l4 8h-22Z"/>
      <path className="v-accent" d="M326 138h26v7h-26zm3 8h26v6h-26z"/>
    </>,
    foreground: <>
      <path className="v-paper v-outline" d="M217 135q13-4 27 3 15-7 28-3l5 15q-17-5-33 1-16-6-32-1Z"/>
      <path className="v-line" d="M244 139v12m-23-12 17 2m-16 2 15 1m14-2 16-3m-15 7 16-3"/>
      <Cue x={244} y={138} pose="scaleX(-.82) skewY(10deg)">
        <path className="v-paper v-outline" d="M0 0q14-6 24-4l3 12Q15 6 0 13Z"/>
        <path className="v-line" d="m6 3 14-3M6 7l14-3"/>
        <path className="v-accent" d="M19-7h3v10l-1-1-2 1Z"/>
      </Cue>
    </>,
  },
  ferry: {
    label: '三途河 · 小町渡河',
    cast: [{character:'komachi', at:[227,131], action:'ferry'}],
    artwork: <>
      <path className="v-wall" d="M0 125q53-29 112-14t133 0 119 2 116-2v85H0Z"/>
      <path className="v-shadow" d="M0 98q43 1 83 29 30 20 83 28-44 23-166 12Zm480 2q-54 14-86 42-22 19-56 26 70 22 142 8Z"/>
      <path className="v-line" opacity=".6" d="M90 129h51m17 0h27m128-9h47m-239 43h45m159 26h59m15 0h28m-284 4h78m16 0h47"/>
      <SpiderLily x={57} y={142} size={.66}/><SpiderLily x={76} y={147} size={.48}/>
      <SpiderLily x={398} y={154} size={.53}/><SpiderLily x={417} y={161} size={.69}/>
      <Cue x={263} y={172} pose="scaleX(1.2)" dim={.12}>
        <path className="v-line" d="M-13 0q23 8 50 1m-27 7q17 3 31-1"/>
      </Cue>
    </>,
    foreground: <g data-scene-motion="ferry">
      <path className="v-wood v-outline" d="M181 145q41 9 89 2l18-8-16 23q-42 13-79 0Z"/>
      <path className="v-line" d="M196 155q38 6 72 0m-31-4 26 22"/>
      <path className="v-wood" d="m260 171 3-2 8 8-3 2Z"/>
    </g>,
  },
  pharmacy: {
    label: '永远亭 · 永琳配药',
    cast: [{character:'eirin', at:[194,121]}, {character:'reisen', at:[329,116]}],
    artwork: <>
      <path className="v-wall" d="M36 160V56h408v104Z"/>
      <path className="v-shadow" d="M0 166h480v35H0ZM70 56h84v83H70z"/>
      <path className="v-line" d="M70 56h84v83H70Zm0 27h84m-84 27h84m-58-52v79m32-79v79M38 160h406m-387 32h366"/>
      <path className="v-gold" d="M78 64h8v10h-8zm26 0h13v10h-13zm29 0h12v10h-12zM78 92h8v8h-8zm26 0h13v8h-13zm29 0h12v8h-12z"/>
      <circle className="v-shadow v-outline" cx="335" cy="91" r="51"/>
      <path className="v-paper" d="M356 58a28 28 0 1 0 8 40 27 27 0 0 1-8-40Z"/>
      <path className="v-line" d="M286 91h98m-49-51v102m-39-70h77m-77 38h77"/>
      <path className="v-wood" d="M166 147h132v9H166Zm8 9h6v34h-6Zm112 0h6v34h-6Z"/>
      <path className="v-accent v-outline" d="M218 145v-9l7-8v-10h10v10l6 8v9Z"/>
      <path className="v-paper v-outline" d="M250 126h13v6l4 5v8h-21v-8l4-5Z"/>
      <path className="v-line" d="M249 140h15m11-5h13l-3 10h-7Z"/>
      <Cue x={257} y={111} pose="translateY(25px)" dim={.25}>
        <path className="v-accent" d="M0 0q-6 7 0 8 6-1 0-8Z"/>
      </Cue>
    </>,
  },
  pump: {
    label: '玄武涧 · 河童水泵',
    cast: [{character:'nitori', at:[194,126]}],
    artwork: <>
      <path className="v-wall" d="M0 126 59 76l54 23 35-43 52 31 35-18 49 31 35-46 56 42 35-12 70 30v73H0Z"/>
      <path className="v-shadow" d="M0 147 47 119l97 20 26 18h173l28-31 57-13 52 25v46H0Z"/>
      <path className="v-accent" d="M110 214q64-36 189-42 52-3 104-23l38 10q-65 36-127 44l-31 11Z" opacity=".65"/>
      <path className="v-line" d="M169 201q49-17 103-18m29 8 68-16m32 2 26-15"/>
      <path className="v-wood" d="M164 154h124v7H164Zm8 7h7v16h-7Zm103 0h7v15h-7Z"/>
      <rect className="v-accent v-outline" x="223" y="128" width="43" height="26" rx="4"/>
      <path className="v-wood v-outline" d="M266 135h20q9 0 9 9v10h-7v-10q0-2-2-2h-20Z"/>
      <circle className="v-paper v-outline" cx="243" cy="119" r="10"/>
      <Cue x={243} y={119} pose="rotate(27deg)">
        <path className="v-hand" d="m0 0-4-5"/>
      </Cue>
      <Cue x={292} y={159} pose="translate(4px,7px) scale(1.3)" dim={.15} delay={260}>
        <circle className="v-line" r="3"/>
      </Cue>
    </>,
  },
  puppets: {
    label: '爱丽丝的工房 · 人偶试线',
    cast: [{character:'alice', at:[194,121]}, workDoll, ...wallDolls],
    artwork: <>
      <path className="v-wall" d="M28 164V73l41-28h342l41 28v91Z"/>
      <path className="v-shadow" d="M12 168h456l12 29H0Z"/>
      <path className="v-line" d="M47 163V79l32-25h322l32 25v84M48 189h385"/>
      <path className="v-shadow v-outline" d="M66 66h83v64H66zm266 0h83v64h-83z"/>
      <path className="v-wood v-outline" d="M69 72h77v5H69zm266 0h77v5h-77z"/>
      {wallDolls.map(({at:[x]}) => <g key={x} transform={`translate(${x} 75)`}>
        <path className="v-line" d="M0 0q-4 4 0 7l-6 10m6-10 6 10"/>
        <circle className="v-gold" r="1.65"/>
      </g>)}
      <path className="v-wood" d="M160 154h157v8H160Zm9 8h7v31h-7Zm130 0h7v31h-7Z"/>
      <path className="v-paper v-outline" d="M299 140h11v4h-11zm-2 5h15v7h-15z"/>
      <path className="v-accent v-outline" d="M216 141h12l-2 3v6l2 2h-12l2-2v-6Zm15 5h7l-1 4 1 2h-7l1-2Z"/>
      <g transform={`translate(${workDoll.pivot[0]} ${workDoll.pivot[1]})`}>
        <g data-scene-motion="puppet">
          <path className="v-wood v-outline" d="M-19-2h38v4h-38Z"/>
          <path className="v-line" d="M-12 2l2 35m22-35-2 35"/>
          <circle className="v-gold" r="2"/>
        </g>
      </g>
    </>,
  },
  concert: {
    label: '花映塚 · 骚灵三重奏',
    cast: [{character:'lunasa', at:[144,115]}, {character:'merlin', at:[239,105]}, {character:'lyrica', at:[337,117]}],
    artwork: <>
      <path className="v-wall" d="M41 138V77q37-37 104-32 52-22 95-13 43-9 95 13 67-5 104 32v61Z"/>
      <path className="v-shadow" d="M68 66v87H26l22-60Zm344 0v87h42l-22-60ZM19 154h442l19 35H0Z"/>
      <path className="v-line" d="M18 178h444m-433 12h422"/>
      <g className="v-gold" opacity=".65"><circle cx="141" cy="61" r="3"/><circle cx="239" cy="54" r="3"/><circle cx="338" cy="61" r="3"/></g>
      <path className="v-wood v-outline" d="M144 136q-10-5-10 1-8 9 1 12 9 1 12-7l5-14-3-1Z"/>
      <Cue x={141} y={139} pose="translate(3px,-2px)">
        <path className="v-line" d="m-18 11 36-22"/>
      </Cue>
      <path className="v-gold v-outline" d="M216 132h35l12-9v22l-12-8h-24q-7 12 13 9l6-9h4q-2 15-20 14-12-1-10-14h-4Z"/>
      <Cue x={265} y={135} pose="translateX(3px)" rest={0} delay={240}>
        <path className="v-spark" d="M4 0h8m-10-5 5-3m-5 13 5 3"/>
      </Cue>
      <path className="v-wood v-outline" d="M309 136h55v13h-55Zm4 13h3v11h-3Zm44 0h3v11h-3Z"/>
      <path className="v-paper" d="M313 138h5v8h-5zm7 0h5v8h-5zm7 0h5v8h-5zm14 0h5v8h-5zm7 0h5v8h-5zm7 0h5v8h-5z"/>
      <Cue x={334} y={138} pose="translateY(2px)" delay={480}>
        <rect className="v-paper" width="5" height="8"/>
      </Cue>
    </>,
  },
  umbrella: {
    label: '人间之里 · 小伞扑空',
    cast: [{character:'kogasa', at:[275,134]}],
    stinger: 'kogasa',
    artwork: <>
      <path className="v-wall" d="M12 169V81l166-40 81 26 148-29 61 39v92Z"/>
      <path className="v-shadow" d="M9 84 178 36l91 31-14 10-83-26L9 99Zm248-15 151-36 70 43-10 13-61-41-150 35Z"/>
      <rect className="v-paper" x="49" y="87" width="157" height="74"/>
      <Cue x={115} y={125} pose="translateX(18px)" dim={.15}>
        <path className="v-shadow" opacity=".3" d="M-9 8v-14a10 10 0 0 1 20 0V8l4 13h-28Z"/>
      </Cue>
      <path className="v-line" d="M49 87h157v74H49Zm0 24h157m-157 25h157M78 87v74m44-74v74m44-74v74M8 173h464m-445 19h426"/>
      <path className="v-accent" d="M222 87h22v81h-22Zm126 9h71v33h-71Z"/>
      <path className="v-line" d="M355 100v25m13-25v25m12-25v25m13-25v25m12-25v25"/>
    </>,
    foreground: <Cue x={300} y={132} pose="translate(-8px,-2px) rotate(-5deg)" delay={280}>
      <path className="v-accent v-outline" d="M-20 0q18-37 41 0L11-3 0 0-10-3Z"/>
      <path className="v-paper" d="M-5-9q5-6 10 0-5 6-10 0Z"/>
      <path className="v-hand" d="M0-11v4m2 7v13q1 6-5 5"/>
    </Cue>,
  },
  lookout: {
    label: '妖怪之山 · 椛值哨',
    cast: [{character:'momiji', at:[241,124], action:'notice'}],
    artwork: <>
      <path className="v-wall" d="m0 126 56-54 44 23 73-60 58 66 39-25 43 31 47-48 59 38 32-10 29 22v76H0Z"/>
      <path className="v-shadow" d="M0 170q65-15 104-42 37-26 87-19l42 20 59 14q93 11 188-35v88H0Z"/>
      <path className="v-paper" opacity=".7" d="M301 132q58 5 140-21-41 30-104 35Zm-263 16q36-10 77-38-23 30-58 37Z"/>
      <path className="v-wood" d="M116 166h246v16H116Z"/>
      <path className="v-line" d="M116 167h246m-238 12h226m-219-19v30m38-31v29m35-28v28m68-28v28m34-29v29m34-28v30"/>
      <path className="v-wood v-outline" d="M170 105h6v61h-6zm129 0h6v61h-6zm-132 1 15-14h111l15 14Z"/>
      <Cue x={275} y={128} pose="translate(-9px,3px) rotate(-12deg)">
        <path className="v-feather v-outline" d="M0 0q2-12 18-15-2 15-16 14l-5 4Z"/>
        <path className="v-line" d="m2-2 11-8"/>
      </Cue>
    </>,
  },
  sunflowers: {
    label: '太阳花田 · 幽香看花',
    cast: [{character:'yuuka', at:[244,126]}],
    artwork: <>
      <path className="v-sky" d="M0 148V99q47-27 111-24 44-29 129-24 61-36 129-9 68-9 111 26v84Z"/>
      <path className="v-wall" d="M0 148q58-39 141-18t126-4q87-35 213-3v71H0Z"/>
      <circle className="v-gold" opacity=".72" cx="338" cy="62" r="26"/>
      <path className="v-paper" opacity=".7" d="M73 81q12-16 28-9 13-9 27 4 12-2 18 7H73Zm284 12q12-11 29-8 9-7 24 7Z"/>
      <path className="v-green" opacity=".45" d="M0 160q88 13 149-2t133 2 112-4 86-6v35H0Z"/>
      <path className="v-shadow" d="M0 182q73 8 158-3t150 9 172-10v26H0Z"/>
      <path className="v-line" opacity=".5" d="M27 201h56m20 0h58m150-2h65m25 0h50"/>
      <Sunflower x={65} y={179} size={.66} delay={0}/>
      <Sunflower x={104} y={173} size={.82} delay={30}/>
      <Sunflower x={152} y={183} size={1} delay={60}/>
      <Sunflower x={196} y={179} size={.73} delay={90}/>
      <Sunflower x={280} y={180} size={.95} delay={430}/>
      <Sunflower x={336} y={184} size={1.07} delay={150}/>
      <Sunflower x={389} y={174} size={.84} delay={180}/>
      <Sunflower x={424} y={182} size={.63} delay={210}/>
    </>,
  },
} satisfies Record<string, LoadingSceneSpec>;
