import {createHmac,timingSafeEqual} from 'node:crypto';

function sign(key,body) {return createHmac('sha256',key).update('room-nickname-v1:').update(body).digest();}
export function issueRoomNickname(key,challenge,name,account) {
  const identity=account?[account.userId,account.sessionId]:[];
  if(account?.profile)identity.push(account.profile);
  const body=Buffer.from(JSON.stringify([challenge,name,Date.now()+30000,...identity])).toString('base64url');
  return body+'.'+sign(key,body).toString('base64url');
}
export function roomNickname(key,challenge,ticket) {
  return roomAccount(key,challenge,ticket)?.name ?? null;
}
export function roomAccount(key,challenge,ticket) {
  if (!key || !challenge || typeof ticket !== 'string' || ticket.length > 1024) return null;
  try {
    const [body,signature,...rest]=ticket.split('.');
    const bytes=Buffer.from(signature,'base64url'), expected=sign(key,body);
    if (rest.length || bytes.length!==expected.length || !timingSafeEqual(bytes,expected)) return null;
    const [nonce,name,expires,userId,sessionId,profile]=JSON.parse(Buffer.from(body,'base64url').toString('utf8'));
    if(nonce!==challenge||!Number.isSafeInteger(expires)||expires<=Date.now()||typeof name!=='string'||!name)return null;
    const account={name,userId:typeof userId==='string'?userId:null,sessionId:typeof sessionId==='string'?sessionId:null};
    if(account.userId&&account.sessionId&&typeof profile?.username==='string'&&
      /^[a-z0-9_.-]{1,32}$/.test(profile.username)&&profile.username!=='.'&&profile.username!=='..'){
      const th09=profile.th09;
      return {...account,profile:{username:profile.username,
        ...(typeof profile.avatar==='string'&&/^[a-f0-9]{16}$/.test(profile.avatar)?{avatar:profile.avatar}:{}),
        ...(Number.isSafeInteger(th09?.games)&&Number.isSafeInteger(th09?.wins)&&th09.wins>=0&&th09.wins<=th09.games
          ?{th09:{games:th09.games,wins:th09.wins}}:{})}};
    }
    return account;
  } catch {return null;}
}
