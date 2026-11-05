import {z} from 'zod';
import {isGameId, isProductId, type GameId} from '../../src/contracts/product-catalog.mts';
import {accountRequest, AccountRequestError} from './account-client';

const count=z.number().int().nonnegative().max(Number.MAX_SAFE_INTEGER).nullable(),time=z.number().finite().nonnegative().nullable();
const mode=z.enum(['pointdevice','legacy']).optional();
const identity={character:z.number().int().min(0).max(63),shot:z.number().int().min(0).max(63).nullable(),mode};
const statistics=z.object({game:z.custom<GameId>(isGameId),parserVersion:z.literal(1),format:z.string(),complete:z.boolean(),
  playtimeSeconds:time,applicationTimeSeconds:time,playCount:count,clearCount:count,oneCreditClearCount:count.default(null),
  loadouts:z.array(z.object({...identity,playtimeSeconds:time,playCount:count})).max(128),
  progress:z.array(z.object({...identity,difficulty:z.enum(['easy','normal','hard','lunatic','extra','phantasm','overdrive']),cleared:z.boolean().nullable(),oneCreditClear:z.boolean().nullable(),clearCount:count,playCount:count,bestScore:count,playtimeSeconds:time})).max(1024),
  warnings:z.array(z.string()).max(6)});
const productId=z.custom<string>(value=>typeof value==='string'&&isProductId(value));
const fileInfo=z.object({name:z.string(),bytes:z.number().int().positive(),sha256:z.string().regex(/^[a-f0-9]{64}$/)});
const snapshot=z.object({product:productId,revision:z.number().int().positive(),uploadedAt:z.string().datetime(),files:z.array(fileInfo).max(6),statistics});
const pendingSession=z.object({product:productId,deviceId:z.string().uuid(),runId:z.string().uuid(),baseRevision:z.number().int().nonnegative(),startedAt:z.string().datetime()});
const listing=z.object({products:z.array(z.object({id:productId,game:z.custom<GameId>(isGameId),multiplayer:z.boolean(),files:z.array(z.string()),requiredFiles:z.array(z.string()),revision:z.number().int().nonnegative()})).max(32),saves:z.array(snapshot).max(64),pending:z.array(pendingSession).max(512).default([]),limits:z.object({fileBytes:z.number().positive(),totalBytes:z.number().positive(),files:z.number().int().positive().max(6)})});
const profile=z.object({career:z.boolean(),records:z.boolean(),playtimeSeconds:time,games:z.array(z.object({product:productId,uploadedAt:z.string().datetime(),statistics})).max(16)});
export type CloudSaveList=z.infer<typeof listing>;
export type CloudSaveSnapshot=z.infer<typeof snapshot>;
export type SaveStatistics=z.infer<typeof statistics>;
export type SaveProgress=SaveStatistics['progress'][number];
export type ProfileStatistics=z.infer<typeof profile>;
export function cloudSaveFileName(allowed:readonly string[],name:string) {
  const normalized=name.normalize('NFC').toLowerCase();
  const matches=allowed.filter(path=>{const value=path.normalize('NFC').toLowerCase();return value===normalized||value.slice(value.lastIndexOf('/')+1)===normalized;});
  return matches.length===1?matches[0]:undefined;
}
function parse<S extends z.ZodTypeAny>(schema:S,value:unknown):z.output<S> {const result=schema.safeParse(value);if(!result.success)throw new AccountRequestError('ACCOUNT_RESPONSE_INVALID');return result.data;}
export async function listCloudSaves(signal?:AbortSignal) {return parse(listing,await accountRequest('v1/me/cloud-saves',undefined,{signal}));}
export async function uploadCloudSave(product:string,revision:number,files:File[],signal?:AbortSignal,session?:{deviceId:string;runId:string}) {
  const form=new FormData();form.append('baseRevision',String(revision));for(const file of files)form.append('files',file,file.name);
  if(session){form.append('deviceId',session.deviceId);form.append('runId',session.runId);}
  return parse(snapshot,await accountRequest('v1/me/cloud-saves/'+product,form,{signal,timeoutMs:90_000}));
}
export async function deleteCloudSave(product:string,baseRevision:number,signal?:AbortSignal) {
  return parse(z.object({revision:z.number().int().nonnegative()}),await accountRequest('v1/me/cloud-saves/'+product+'/delete',{baseRevision},{signal}));
}
export async function downloadCloudSave(product:string,revision:number,signal?:AbortSignal) {
  return accountRequest<Blob>(`v1/me/cloud-saves/${product}/${revision}/download`,undefined,{signal,responseType:'blob',timeoutMs:90_000});
}
export async function getProfileStatistics(username:string,signal?:AbortSignal) {return parse(profile,await accountRequest(`v1/profiles/${encodeURIComponent(username)}/statistics`,undefined,{signal}));}
export function formatPlaytime(seconds:number,locale:string) {
  const minutes=Math.floor(seconds/60),hours=Math.floor(minutes/60),formatter=new Intl.NumberFormat(locale);
  if(minutes===0)return seconds>0&&seconds<1?'<1s':formatter.format(Math.floor(seconds))+'s';
  return hours?`${formatter.format(hours)}h ${minutes%60}m`:`${formatter.format(minutes)}m`;
}
