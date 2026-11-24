import {useEffect, useRef, useState} from 'react';
import {ArrowLeft, Mail, RefreshCw} from 'lucide-react';
import {useLocale} from '../../i18n';
import {AccountRequestError} from '../../services/account-client';
import {
  fetchConversation, fetchInbox, inboxChanged, markConversationRead, observeMessageUpdates,
  sendDirectMessage, validMessageUsername, type ConversationPage, type InboxPage,
} from '../../services/launcher-messages';
import {Button} from '../../ui/button';
import {Input} from '../../ui/input';
import {MessageComposer} from './MessageComposer';

export function PrivateMessages({initialPeer}: {initialPeer?: string}) {
  const [peer, setPeer] = useState(initialPeer);
  return peer ? <Conversation key={peer} username={peer} back={() => setPeer(undefined)}/> :
    <Inbox open={setPeer}/>;
}

function Inbox({open}: {open: (username: string) => void}) {
  const {t} = useLocale();
  const [username, setUsername] = useState('');
  const [pages, setPages] = useState<InboxPage[]>([]);
  const [error, setError] = useState(false);
  const [loadingMore, setLoadingMore] = useState(false);
  useEffect(() => {
    let pending: AbortController | undefined;
    function refresh() {
      pending?.abort();
      const controller = new AbortController();
      pending = controller;
      void fetchInbox(undefined, controller.signal).then(page => {
        if (controller.signal.aborted) return;
        setPages(previous => [page, ...previous.slice(1)]);
        setError(false);
      }).catch(() => {if (!controller.signal.aborted) setError(true);});
    }
    refresh();
    const cleanup = observeMessageUpdates(inboxChanged, refresh, 30_000);
    return () => {pending?.abort(); cleanup();};
  }, []);

  const next = pages.at(-1)?.nextBefore;
  const threads = [...new Map(pages.flatMap(page => page.threads).reverse().map(thread => [thread.username, thread])).values()]
    .sort((a, b) => b.lastMessage.id - a.lastMessage.id);
  const target = username.trim().replace(/^@/, '').toLowerCase();
  return <div className="private-inbox">
    <form className="private-message-controls" onSubmit={event => {
      event.preventDefault();
      if (validMessageUsername(target)) open(target);
    }}>
      <Input value={username} onChange={event => setUsername(event.target.value)} autoCapitalize="none"
        aria-label={t('messages.username')} placeholder={t('messages.username')} maxLength={33}/>
      <Button variant="outline" type="submit" disabled={!validMessageUsername(target)}>{t('messages.openConversation')}</Button>
      <Button variant="ghost" size="icon" type="button" aria-label={t('messages.refresh')}
        onClick={() => window.dispatchEvent(new Event(inboxChanged))}><RefreshCw aria-hidden="true"/></Button>
    </form>
    {error && <p role="status" className="message-request-error">{t('messages.loadFailed')}</p>}
    {!pages.length && !error && <p role="status">{t('messages.loading')}</p>}
    {pages.length > 0 && !threads.length && <div className="message-empty"><Mail aria-hidden="true"/><p>{t('messages.noConversations')}</p></div>}
    <ul className="private-thread-list">
      {threads.map(({username: name, nickname, lastMessage, unread}) => <li key={name}>
        <Button variant="ghost" className="private-thread" onClick={() => open(name)}>
          <span><strong>{nickname}</strong> <small>@{name}</small></span>
          <span className="private-thread-preview">{lastMessage.body}</span>
          {unread > 0 && <>
            <span className="private-thread-badge" aria-hidden="true">{unread}</span>
            <span className="sr-only">{t('messages.unread', {count: unread})}</span>
          </>}
        </Button>
      </li>)}
    </ul>
    {next && <Button variant="outline" disabled={loadingMore} onClick={async () => {
      setLoadingMore(true);
      try {const page = await fetchInbox(next); setPages(previous => [...previous, page]); setError(false);}
      catch {setError(true);}
      finally {setLoadingMore(false);}
    }}>{t('messages.older')}</Button>}
  </div>;
}

const conversationChanged = 'launcher-conversation-refresh';

function Conversation({username, back}: {username: string; back: () => void}) {
  const {locale, t} = useLocale();
  const [pages, setPages] = useState<ConversationPage[]>([]);
  const [error, setError] = useState<'missing' | 'load'>();
  const [loadingMore, setLoadingMore] = useState(false);
  const readThrough = useRef(0);
  useEffect(() => {
    let pending: AbortController | undefined;
    function refresh() {
      if (document.hidden) return;
      pending?.abort();
      const controller = new AbortController();
      pending = controller;
      void fetchConversation(username, undefined, controller.signal).then(page => {
        if (controller.signal.aborted) return;
        setPages(previous => {
          if (!previous.length || previous.length === 1 && page.nextBefore === null) return [page];
          const latest = previous[0];
          const known = new Map([...latest.messages, ...page.messages].map(message => [message.id, message]));
          return [{...page, messages: [...known.values()].sort((a, b) => a.id - b.id),
            nextBefore: latest.nextBefore ?? page.nextBefore}, ...previous.slice(1)];
        });
        setError(undefined);
        const incoming = page.messages.findLast(message => !message.outgoing);
        if (incoming && incoming.id > readThrough.current && !document.hidden)
          void markConversationRead(username, incoming.id, controller.signal)
            .then(() => {readThrough.current = Math.max(readThrough.current, incoming.id);}).catch(() => {});
      }).catch(cause => {
        if (!controller.signal.aborted)
          setError(cause instanceof AccountRequestError && cause.status === 404 ? 'missing' : 'load');
      });
    }
    refresh();
    const cleanup = observeMessageUpdates(conversationChanged, refresh, 30_000);
    return () => {pending?.abort(); cleanup();};
  }, [username]);

  const first = pages[0];
  const next = pages.at(-1)?.nextBefore;
  const messages = [...new Map(pages.flatMap(page => page.messages).map(message => [message.id, message])).values()]
    .sort((a, b) => a.id - b.id);
  const format = new Intl.DateTimeFormat(locale, {dateStyle: 'medium', timeStyle: 'short'});
  return <div className="private-conversation">
    <header className="private-message-controls">
      <Button variant="ghost" size="icon" aria-label={t('messages.backToInbox')} onClick={back}><ArrowLeft aria-hidden="true"/></Button>
      <h2>{first ? first.peer.nickname : '@' + username} {first && <small>@{first.peer.username}</small>}</h2>
      <Button variant="ghost" size="icon" aria-label={t('messages.refresh')}
        onClick={() => window.dispatchEvent(new Event(conversationChanged))}><RefreshCw aria-hidden="true"/></Button>
    </header>
    {error && <p role="status" className="message-request-error">{t(error === 'missing' ? 'messages.userMissing' : 'messages.loadFailed')}</p>}
    {!first && !error && <p role="status">{t('messages.loading')}</p>}
    {next && <Button variant="outline" disabled={loadingMore} onClick={async () => {
      setLoadingMore(true);
      try {const page = await fetchConversation(username, next); setPages(previous => [...previous, page]); setError(undefined);}
      catch {setError('load');}
      finally {setLoadingMore(false);}
    }}>{t('messages.older')}</Button>}
    {messages.length > 0 && <ol className="private-message-list">{messages.map(({id, sentAt, body, outgoing}) =>
      <li key={id} data-outgoing={outgoing}>
        <p>{body}</p><time dateTime={new Date(sentAt).toISOString()}>{format.format(sentAt)}</time>
      </li>)}</ol>}
    {first && error !== 'missing' && <MessageComposer label="messages.write" action="messages.send" limit={2000}
      send={async (body, requestId) => {
        await sendDirectMessage(username, body, requestId);
        window.dispatchEvent(new Event(conversationChanged));
      }}/>}
  </div>;
}
