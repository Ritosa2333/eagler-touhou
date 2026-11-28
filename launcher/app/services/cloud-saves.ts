import {z} from 'zod';
import {isGameId, isMultiplayerProductId, isProductId, type GameId} from '../../src/contracts/product-catalog.mts';
import {accountRequest, AccountRequestError} from './account-client';

const count=z.number().int().nonnegative().max(Number.MAX_SAFE_INTEGER).nullable(),time=z.number().finite().nonnegative().nullable();
const mode=z.enum(['pointdevice','legacy']).optional();
const identity={character:z.number().int().min(0).max(63),shot:z.number().int().min(0).max(63).nullable(),mode};
const spellCard=z.object({
  number:z.number().int().min(1).max(141),name:z.string().max(49).optional(),
  attempts:z.array(z.number().int().min(0).max(65535)).length(6),captures:z.array(z.number().int().min(0).max(65535)).length(6),
});
const spellCount = z.number().int().min(0).max(2147483647);
const spellTally = z.object({attempts:spellCount,captures:spellCount});
const spellArchive = z.object({
  scope:z.enum(['total','loadout']),
  cards:z.array(z.object({
    number:z.number().int().min(1).max(512),name:z.string().max(49).optional(),
    total:spellTally.optional(),
    loadouts:z.array(spellTally.extend({
      character:identity.character,shot:identity.shot,
      difficulty:z.enum(['easy','normal','hard','lunatic','extra','phantasm','overdrive','last-word']),
      activity:z.enum(['story','practice']).optional(),
    })).max(128).optional(),
  })).max(512),
});
const statistics=z.object({game:z.custom<GameId>(value=>typeof value==='string'&&isGameId(value)),parserVersion:z.literal(1),format:z.string(),complete:z.boolean(),
  playtimeSeconds:time,applicationTimeSeconds:time,playCount:count,clearCount:count,oneCreditClearCount:count.default(null),
  loadouts:z.array(z.object({...identity,playtimeSeconds:time,playCount:count})).max(128),
  progress:z.array(z.object({...identity,difficulty:z.enum(['easy','normal','hard','lunatic','extra','phantasm','overdrive']),cleared:z.boolean().nullable(),oneCreditClear:z.boolean().nullable(),clearCount:count,playCount:count,bestScore:count,playtimeSeconds:time})).max(1024),
  spellCards:z.array(spellCard).max(141).optional(),
  spellArchive:z.unknown().optional(),
  warnings:z.array(z.string()).max(6)}).transform(value=>{
    // Optional display statistics must not block access to the save files.
    const archive=spellArchive.optional().safeParse(value.spellArchive);
    return {...value,spellArchive:archive.success?archive.data:undefined,
      warnings:archive.success?value.warnings:[...new Set([...value.warnings,'spell-archive-unavailable'])]};
  });
const productId=z.custom<string>(value=>typeof value==='string'&&isProductId(value));
const fileInfo=z.object({name:z.string(),bytes:z.number().int().positive(),sha256:z.string().regex(/^[a-f0-9]{64}$/)});
const snapshot=z.object({product:productId,revision:z.number().int().positive(),uploadedAt:z.string().datetime(),files:z.array(fileInfo).max(6),statistics});
const pendingSession=z.object({product:productId,deviceId:z.string().uuid(),runId:z.string().uuid(),baseRevision:z.number().int().nonnegative(),startedAt:z.string().datetime()});
const listing=z.object({products:z.array(z.object({id:productId,game:z.custom<GameId>(value=>typeof value==='string'&&isGameId(value)),multiplayer:z.boolean(),files:z.array(z.string()),requiredFiles:z.array(z.string()),revision:z.number().int().nonnegative()})).max(32),saves:z.array(snapshot).max(64),pending:z.array(pendingSession).max(512).default([]),limits:z.object({fileBytes:z.number().positive(),totalBytes:z.number().positive(),files:z.number().int().positive().max(6)})});
const profile=z.object({career:z.boolean(),records:z.boolean(),playtimeSeconds:time,games:z.array(z.object({product:productId,uploadedAt:z.string().datetime(),statistics})).max(16)});
export type CloudSaveList=z.infer<typeof listing>;
export type CloudSaveSnapshot=z.infer<typeof snapshot>;
export type SaveStatistics=z.infer<typeof statistics>;
export type SaveProgress=SaveStatistics['progress'][number];
export type SpellCardCapture=z.infer<typeof spellCard>;
export type SpellArchive=z.infer<typeof spellArchive>;
export type ProfileStatistics=z.infer<typeof profile>;
export function supportsCloudSaveSync(product: string) {
  // TH17 writes version 2 records, which the cloud parser rejects.
  return isProductId(product) && product !== 'th17' && !isMultiplayerProductId(product);
}
export function cloudSaveFileName(allowed:readonly string[],name:string) {
  const normalized=name.normalize('NFC').toLowerCase();
  const matches=allowed.filter(path=>{const value=path.normalize('NFC').toLowerCase();return value===normalized||value.slice(value.lastIndexOf('/')+1)===normalized;});
  return matches.length===1?matches[0]:undefined;
}
function parse<S extends z.ZodTypeAny>(schema:S,value:unknown):z.output<S> {const result=schema.safeParse(value);if(!result.success)throw new AccountRequestError('ACCOUNT_RESPONSE_INVALID');return result.data;}
export async function listCloudSaves(signal?:AbortSignal) {return parse(listing,await accountRequest('v1/me/cloud-saves',undefined,{signal}));}
export async function uploadCloudSave(product:string,revision:number,files:File[],signal?:AbortSignal) {
  const form=new FormData();form.append('baseRevision',String(revision));for(const file of files)form.append('files',file,file.name);
  return parse(snapshot,await accountRequest('v1/me/cloud-saves/'+product,form,{signal,timeoutMs:90_000}));
}
export async function previewCloudSave(product:string,files:File[],signal?:AbortSignal) {
  const form=new FormData();for(const file of files)form.append('files',file,file.name);
  return parse(z.object({product:productId,files:z.array(fileInfo).max(6),statistics}),await accountRequest('v1/me/cloud-saves/'+product+'/preview',form,{signal,timeoutMs:2500}));
}
export async function deleteCloudSave(product:string,baseRevision:number,signal?:AbortSignal) {
  return parse(z.object({revision:z.number().int().nonnegative()}),await accountRequest('v1/me/cloud-saves/'+product+'/delete',{baseRevision},{signal}));
}
export async function downloadCloudSave(product:string,revision:number,signal?:AbortSignal) {
  return accountRequest<Blob>(`v1/me/cloud-saves/${product}/${revision}/download`,undefined,{signal,responseType:'blob',timeoutMs:90_000});
}
export async function getProfileStatistics(username:string,signal?:AbortSignal,anonymous=false) {return parse(profile,await accountRequest(`v1/profiles/${encodeURIComponent(username)}/statistics`,undefined,{signal,anonymous}));}
export function formatPlaytime(seconds:number,locale:string) {
  const minutes=Math.floor(seconds/60),hours=Math.floor(minutes/60);
  const format = (value:number,unit:'hour'|'minute'|'second') => {
    if (locale === 'zh-CN') {
      const label = unit === 'hour' ? '小时' : unit === 'second' ? '秒' : hours ? '分' : '分钟';
      return new Intl.NumberFormat(locale).format(value) + ' ' + label;
    }
    return new Intl.NumberFormat(locale,{style:'unit',unit,unitDisplay:'short'}).format(value);
  };
  if (minutes===0) return seconds>0 && seconds<1 ? '<' + format(1,'second') : format(Math.floor(seconds),'second');
  return hours ? [format(hours,'hour'), minutes%60 ? format(minutes%60,'minute') : ''].filter(Boolean).join(' ') : format(minutes,'minute');
}
