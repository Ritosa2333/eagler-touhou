import {useEffect, useState} from 'react';
import {accountRequest} from './account-client';

type Identity = {username: string; name: string};
type State = {status:'loading'|'guest'|'error'} | {status:'member'; user:Identity};
const signalKey = 'account-session-changed';
let pending: Promise<State> | undefined;

function readSession() {
  pending ??= accountRequest<{user?:Identity} | null>('auth/get-session')
    .then((session):State => session?.user?.username ? {status:'member',user:session.user} : {status:'guest'})
    .catch(():State => ({status:'error'})).finally(() => {pending = undefined;});
  return pending;
}

export function useAccountSession(interactive = true) {
  const [state,setState] = useState<State>({status:'loading'});
  const [attempt,setAttempt] = useState(0);
  useEffect(() => {
    if (!interactive) return;
    let active = true;
    const refresh = () => {void readSession().then(value => {if (active) setState(value);});};
    const changed = (event:StorageEvent) => {if (event.key === signalKey) location.reload();};
    const restored = (event:PageTransitionEvent) => {if (event.persisted) location.reload();};
    refresh();
    window.addEventListener('storage',changed); window.addEventListener('pageshow',restored);
    return () => {active=false;window.removeEventListener('storage',changed);window.removeEventListener('pageshow',restored);};
  },[interactive,attempt]);
  return {state,retry:() => setAttempt(value => value+1)};
}

export function notifyAccountChange() {
  try {localStorage.setItem(signalKey,crypto.randomUUID());} catch {}
}

export function loginDestination() {
  const url = new URL(location.href);
  const next = url.searchParams.get('next');
  if (next?.startsWith('/') && !next.startsWith('//')) {
    const target = new URL(next,url);
    if (target.origin === url.origin && !/^\/login\.html\/?$/.test(target.pathname)) return target.href;
  }
  url.pathname = '/profile.html'; url.searchParams.delete('next');url.searchParams.delete('mode');url.hash = '';
  return url.href;
}
