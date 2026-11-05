import {TH19_VERSUS_RULES} from '../../../src/launcher/th19-versus-rules.mts';
import {useLocale} from '../../i18n';
import {SettingsSelect} from '../settings/SettingsSelect';
export function Th19VersusSettings({rules=TH19_VERSUS_RULES,disabled,onChange}:{rules?:readonly number[];disabled:boolean;onChange(value:number[]):void}){
 const {t}=useLocale();
 const labels=['th19.time','th19.rounds','th19.ability','th19.life1','th19.life2','th19.barrier1','th19.barrier2','th19.card1','th19.card2'] as const;
 const choices=[
  [t('th19.none'),...[3,5,7,10,15,20].map(count=>t('th19.minutes',{count}))],
  ['1','2','3'],
  [t('th19.none'),t('th19.random'),t('th19.self'),t('th19.all')],
  ['1','2','3','4','5'],['1','2','3','4','5'],
  [t('th19.none'),t('th19.manual'),t('th19.long'),t('th19.short')],
  [t('th19.none'),t('th19.manual'),t('th19.long'),t('th19.short')],
  ...[0,1].map(()=>rules[2]===2?[1,2].map(number=>t('th19.selfCard',{number})):[t('th19.none'),...Array.from({length:38},(_,n)=>t('th19.card',{number:n+1}))])
 ];
 return <fieldset className="room-basic-fields" data-th19-versus-settings>{labels.map((label,index)=>index<7||rules[2]>=2?<SettingsSelect key={label} id={'mpTh19Rule'+index} label={t(label)} disabled={disabled} value={rules[index]} onChange={event=>{const next=[...rules];next[index]=Number(event.target.value);if(index===2)next[7]=next[8]=0;onChange(next);}}>{choices[index].map((text,value)=><option key={value} value={value}>{text}</option>)}</SettingsSelect>:null)}</fieldset>;
}
