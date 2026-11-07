import {useSyncExternalStore} from 'react';
import {AccountRequestError, accountRequest, cancelAccountRequests} from './account-client';

type Identity = {username: string; name: string};
type State = {status: 'loading' | 'error'} | {status: 'guest'; expired?: boolean} | {status: 'member'; user: Identity; expiresAt?: number};
const loading: State = {status: 'loading'};
const signalKey = 'account-session-changed';
export const roomProfileChangeKey = 'account-room-profile-changed';
const listeners = new Set<() => void>();
const recheckAfter = 30_000;
let state: State = loading;
let checkedAt = 0;
let generation = 0;
let pending: {controller: AbortController; promise: Promise<void>} | undefined;

function publish(next: State) {
  if (state.status === next.status) {
    if (state.status === 'member' && next.status === 'member') {
      if (state.user.username === next.user.username && state.user.name === next.user.name && state.expiresAt === next.expiresAt) return;
    } else if (state.status !== 'guest' || next.status !== 'guest' || state.expired === next.expired) return;
  }
  const previousUser = state.status === 'member' ? state.user.username : null;
  const nextUser = next.status === 'member' ? next.user.username : null;
  if (previousUser !== nextUser && state.status !== 'loading' && next.status !== 'loading') cancelAccountRequests();
  state = next;
  for (const listener of listeners) listener();
}

function refresh() {
  if (pending) return pending.promise;
  const controller = new AbortController();
  const requestGeneration = ++generation;
  const promise = accountRequest<{user?: Identity; session?: {expiresAt?: string}} | null>('auth/get-session', undefined, {signal: controller.signal})
    .then(session => {
      if (controller.signal.aborted || generation !== requestGeneration) return;
      checkedAt = Date.now();
      const expiresAt = Date.parse(session?.session?.expiresAt ?? '');
      if (session?.user?.username && !(expiresAt <= checkedAt))
        publish({status: 'member', user: session.user, ...(Number.isFinite(expiresAt) ? {expiresAt} : {})});
      else publish(state.status === 'member' || expiresAt <= checkedAt ? {status: 'guest', expired: true} : {status: 'guest'});
    })
    .catch(error => {
      if (controller.signal.aborted || generation !== requestGeneration) return;
      if (error instanceof AccountRequestError && error.status === 401)
        publish(state.status === 'member' ? {status: 'guest', expired: true} : {status: 'guest'});
      // A failed background recheck must not discard an open profile draft.
      else if (state.status === 'loading' || state.status === 'error') publish({status: 'error'});
    })
    .finally(() => {if (generation === requestGeneration) pending = undefined;});
  pending = {controller, promise};
  return promise;
}

function invalidate() {
  generation++;
  pending?.controller.abort();
  pending = undefined;
  cancelAccountRequests();
  checkedAt = 0;
  publish(loading);
  if (listeners.size) void refresh();
}

function changed(event: StorageEvent) {
  if (event.key === signalKey || event.key === null) invalidate();
}

function recheck() {
  if (document.visibilityState !== 'hidden' && Date.now() - checkedAt >= recheckAfter) void refresh();
}

function restored(event: PageTransitionEvent) {
  if (event.persisted) invalidate();
}

function subscribe(listener: () => void) {
  listeners.add(listener);
  if (listeners.size === 1) {
    window.addEventListener('storage', changed);
    window.addEventListener('pageshow', restored);
    window.addEventListener('focus', recheck);
    document.addEventListener('visibilitychange', recheck);
    void refresh();
  }
  return () => {
    listeners.delete(listener);
    if (listeners.size) return;
    window.removeEventListener('storage', changed);
    window.removeEventListener('pageshow', restored);
    window.removeEventListener('focus', recheck);
    document.removeEventListener('visibilitychange', recheck);
    generation++;
    pending?.controller.abort();
    pending = undefined;
    state = loading;
    checkedAt = 0;
  };
}

function expire(username: string) {
  if (state.status !== 'member' || state.user.username.toLowerCase() !== username.toLowerCase()) return;
  generation++;
  pending?.controller.abort();
  pending = undefined;
  checkedAt = Date.now();
  publish({status: 'guest', expired: true});
}

const getSnapshot = () => state;
const getServerSnapshot = () => loading;
const inactiveSubscription = () => () => {};
export const accountSession = {subscribe, getSnapshot, expire, whenReady:()=>state.status==='loading'?refresh():Promise.resolve()};

export function useAccountSession(interactive = true) {
  const snapshot = useSyncExternalStore(interactive ? subscribe : inactiveSubscription,
    interactive ? getSnapshot : getServerSnapshot, getServerSnapshot);
  return {state: snapshot, retry: invalidate};
}

export function notifyAccountChange() {
  invalidate();
  try {localStorage.setItem(signalKey,crypto.randomUUID());} catch {}
}

export function notifyRoomProfileChange() {
  window.dispatchEvent(new Event(roomProfileChangeKey));
  try { localStorage.setItem(roomProfileChangeKey, crypto.randomUUID()); } catch {}
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
