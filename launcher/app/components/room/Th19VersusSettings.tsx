import {TH19_VERSUS_RULES} from '../../../src/launcher/th19-versus-rules.mts';
import {useLocale} from '../../i18n';
import {SettingsSelect} from '../settings/SettingsSelect';
export function Th19VersusSettings({rules=TH19_VERSUS_RULES,disabled,onChange}:{rules?:readonly number[];disabled:boolean;onChange(value:number[]):void}){
 const {t}=useLocale();
 const fields=[
  {index:1,label:t('th19.rounds'),choices:['1','2','3']},
  {index:2,label:t('th19.ability'),choices:[t('th19.none'),t('th19.random'),t('th19.self'),t('th19.all')]},
  {index:3,label:t('th19.lives'),choices:['1','2','3','4','5']},
  ...[7,8].filter(()=>rules[2]>=2).map(index=>({index,label:t(index===7?'th19.card1':'th19.card2'),choices:rules[2]===2?[1,2].map(number=>t('th19.selfCard',{number})):[t('th19.none'),...Array.from({length:38},(_,n)=>t('th19.card',{number:n+1}))]}))
 ];
 return <fieldset className="room-basic-fields" data-th19-versus-settings>{fields.map(({index,label,choices})=><SettingsSelect key={index} id={'mpTh19Rule'+index} label={label} disabled={disabled} value={rules[index]} onChange={event=>{const next=[...rules];next[index]=Number(event.target.value);if(index===3)next[4]=next[3];if(index===2)next[7]=next[8]=0;onChange(next);}}>{choices.map((text,value)=><option key={text} value={value}>{text}</option>)}</SettingsSelect>)}</fieldset>;
}
