import {useEffect, useState} from 'react';
import {z} from 'zod';
import {accountsEnabled} from '../release-features';
import {accountRequest} from './account-client';
import {useAccountSession} from './account-session';

const id = z.number().int().positive();
const nextBefore = id.nullable();
const publishedAnnouncement = z.object({id, author: z.string(), authorUsername: z.string().nullable().optional(), body: z.string(), publishedAt: id});
const directMessage = z.object({id, body: z.string(), sentAt: id, outgoing: z.boolean()});
const peer = z.object({username: z.string(), nickname: z.string()});
const announcementPage = z.object({items: z.array(publishedAnnouncement), canPublish: z.boolean(), nextBefore});
const inboxPage = z.object({
  threads: z.array(peer.extend({lastMessage: directMessage, unread: z.number().int().nonnegative()})),
  unread: z.number().int().nonnegative(),
  nextBefore,
});
const conversationPage = z.object({peer, messages: z.array(directMessage), nextBefore});
const ack = z.object({ok: z.literal(true)});

export type PublishedAnnouncement = z.infer<typeof publishedAnnouncement>;
export type InboxPage = z.infer<typeof inboxPage>;
export type ConversationPage = z.infer<typeof conversationPage>;

const announcementsPath = 'v1/announcements';
const inboxPath = 'v1/me/messages';
const conversationPath = (username: string) => inboxPath + '/' + username;
export const validMessageUsername = (value: string) => /^(?!\.{1,2}$)[\w.-]{1,32}$/.test(value);
export const announcementsChanged = 'launcher-announcements-changed';
export const inboxChanged = 'launcher-inbox-changed';
const announcementKey = (id: number) => 'published:' + id;

function pageQuery(before?: number) {
  return before ? new URLSearchParams({before: String(before)}) : undefined;
}

export async function fetchAnnouncements(before?: number, signal?: AbortSignal) {
  return announcementPage.parse(await accountRequest(announcementsPath, undefined, {query: pageQuery(before), signal}));
}

export async function fetchInbox(before?: number, signal?: AbortSignal, summary = false) {
  const query = summary ? new URLSearchParams({summary: '1'}) : pageQuery(before);
  return inboxPage.parse(await accountRequest(inboxPath, undefined, {query, signal}));
}

export async function fetchConversation(username: string, before?: number, signal?: AbortSignal) {
  return conversationPage.parse(await accountRequest(conversationPath(username), undefined, {query: pageQuery(before), signal}));
}

export async function publishAnnouncement(body: string, requestId: string) {
  ack.parse(await accountRequest(announcementsPath, {body, requestId}));
  window.dispatchEvent(new Event(announcementsChanged));
}

export async function sendDirectMessage(username: string, body: string, requestId: string) {
  ack.parse(await accountRequest(conversationPath(username), {body, requestId}));
  window.dispatchEvent(new Event(inboxChanged));
}

export async function markConversationRead(username: string, through: number, signal: AbortSignal) {
  ack.parse(await accountRequest(conversationPath(username) + '/read', {through}, {signal}));
  window.dispatchEvent(new Event(inboxChanged));
}

export function useMessageIdentity(interactive = true) {
  return useAccountSession(accountsEnabled && interactive);
}

export function observeMessageUpdates(event: string, refresh: () => void, interval?: number) {
  const foregroundRefresh = () => {if (!document.hidden) refresh();};
  window.addEventListener(event, refresh);
  window.addEventListener('focus', foregroundRefresh);
  const timer = interval === undefined ? undefined : window.setInterval(foregroundRefresh, interval);
  return () => {
    window.removeEventListener(event, refresh);
    window.removeEventListener('focus', foregroundRefresh);
    if (timer !== undefined) window.clearInterval(timer);
  };
}

export function usePublishedAnnouncements(username?: string, enabled = true) {
  const [feed, setFeed] = useState<{username?: string; data: z.infer<typeof announcementPage>}>();
  useEffect(() => {
    if (!accountsEnabled || !enabled) return;
    let pending: AbortController | undefined;
    const refresh = () => {
      pending?.abort();
      const controller = new AbortController();
      pending = controller;
      void fetchAnnouncements(undefined, controller.signal).then(data => {
        if (!controller.signal.aborted) setFeed({username, data});
      }).catch(() => {});
    };
    refresh();
    const cleanup = observeMessageUpdates(announcementsChanged, refresh);
    return () => {pending?.abort(); cleanup();};
  }, [username, enabled]);
  return feed?.username === username ? feed?.data : undefined;
}

export function useInboxSummary(username?: string) {
  const [summary, setSummary] = useState<{username: string; unread: number}>();
  useEffect(() => {
    if (!username) return;
    let pending: AbortController | undefined;
    const refresh = () => {
      pending?.abort();
      const controller = new AbortController();
      pending = controller;
      void fetchInbox(undefined, controller.signal, true).then(({unread}) => {
        if (!controller.signal.aborted) setSummary({username, unread});
      }).catch(() => {});
    };
    refresh();
    const cleanup = observeMessageUpdates(inboxChanged, refresh, 30_000);
    return () => {pending?.abort(); cleanup();};
  }, [username]);
  return summary?.username === username ? summary?.unread : undefined;
}

export {announcementKey};
