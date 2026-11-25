import {useRef, useState} from 'react';
import {Send} from 'lucide-react';
import {useLocale, type UiMessageKey} from '../../i18n';
import {AccountRequestError} from '../../services/account-client';
import {Button} from '../../ui/button';
import {Textarea} from '../../ui/input';

export function MessageComposer({label, action, limit, send}: {
  label: UiMessageKey;
  action: UiMessageKey;
  limit: number;
  send: (body: string, requestId: string) => Promise<void>;
}) {
  const {t} = useLocale();
  const [body, setBody] = useState('');
  const [sending, setSending] = useState(false);
  const [error, setError] = useState<UiMessageKey>();
  const request = useRef<{body: string; id: string}>(null);
  return <form className="message-composer" onSubmit={async event => {
    event.preventDefault();
    const text = body.trim();
    if (sending || !text) return;
    if (request.current?.body !== text) request.current = {body: text, id: crypto.randomUUID()};
    setSending(true);
    setError(undefined);
    try {
      await send(text, request.current.id);
      request.current = null;
      setBody('');
    } catch (cause) {
      setError(cause instanceof AccountRequestError && cause.status === 429 ? 'messages.rateLimited' : 'messages.sendFailed');
    } finally {
      setSending(false);
    }
  }}>
    <Textarea value={body} onChange={event => setBody(event.target.value)} aria-label={t(label)}
      placeholder={t(label)} maxLength={limit} rows={3} readOnly={sending} required/>
    <div className="message-composer-actions">
      {error && <p role="alert" className="message-request-error">{t(error)}</p>}
      <Button type="submit" disabled={sending || !body.trim()}><Send aria-hidden="true"/>{t(action)}</Button>
    </div>
  </form>;
}
