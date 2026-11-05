import {useEffect, useRef, useState, type FormEvent} from 'react';
import {ArrowLeft} from 'lucide-react';
import {accountRequest, AccountRequestError} from '../../services/account-client';
import {useLocale, type UiMessageKey} from '../../i18n';
import {Input} from '../../ui/input';
import {BookButton} from './BookButton';
import {PasswordField} from './PasswordField';

export function PasswordResetForm({onBack}: {onBack(): void}) {
  const {t} = useLocale();
  const [email, setEmail] = useState(''), [code, setCode] = useState('');
  const [password, setPassword] = useState(''), [confirmation, setConfirmation] = useState('');
  const [sent, setSent] = useState(false), [done, setDone] = useState(false), [busy, setBusy] = useState(false);
  const [remaining, setRemaining] = useState(0), [error, setError] = useState<UiMessageKey | null>(null);
  const task = useRef<AbortController | null>(null), codeInput = useRef<HTMLInputElement>(null);
  useEffect(() => () => task.current?.abort(), []);
  useEffect(() => {
    if (!remaining) return;
    const timer = setTimeout(() => setRemaining(value => Math.max(0, value - 1)), 1000);
    return () => clearTimeout(timer);
  }, [remaining]);
  useEffect(() => {if (sent) codeInput.current?.focus();}, [sent]);
  const request = async (reset: boolean) => {
    if (busy || (!reset && remaining)) return;
    setError(null);
    if (!/^[^\s@]+@[^\s@]+\.[^\s@]+$/.test(email.trim())) {setError('account.emailInvalid');return;}
    if (reset) {
      if (!/^[0-9]{6}$/.test(code)) {setError('account.codeRequired');return;}
      if (password.length < 8) {setError('account.passwordLength');return;}
      if (password !== confirmation) {setError('account.passwordMismatch');return;}
    }
    const controller = new AbortController();
    task.current = controller; setBusy(true);
    try {
      const result = await accountRequest<{success: boolean}>(`auth/email-otp/${reset ? 'reset-password' : 'request-password-reset'}`,
        reset ? {email: email.trim(), otp: code, password} : {email: email.trim()}, {signal: controller.signal});
      if (result?.success !== true) throw new AccountRequestError('ACCOUNT_RESPONSE_INVALID');
      if (reset) {setDone(true);setPassword('');setConfirmation('');setCode('');}
      else {setSent(true);setRemaining(60);}
    } catch (cause) {
      if (controller.signal.aborted) return;
      const code = cause instanceof AccountRequestError ? cause.code : '';
      setError(cause instanceof AccountRequestError && cause.status === 429 || code === 'TOO_MANY_REQUESTS' ? 'account.tooManyRequests'
        : ['INVALID_OTP', 'INVALID_EMAIL_OTP', 'OTP_EXPIRED', 'VERIFICATION_CODE_EXPIRED'].includes(code) ? 'account.codeInvalid' : 'account.requestFailed');
    } finally {if (!controller.signal.aborted) setBusy(false);}
  };
  const submit = (event: FormEvent<HTMLFormElement>) => {event.preventDefault();void request(sent);};
  return <div className="account-mode-content">
    <BookButton variant="quiet" className="account-back" onClick={onBack}><ArrowLeft aria-hidden="true"/>{t('account.backToLogin')}</BookButton>
    <header><h1 id="account-title">{t('account.resetPassword')}</h1></header>
    {done ? <div className="account-reset-success"><p role="status">{t('account.passwordResetDone')}</p><BookButton variant="primary" className="account-submit" onClick={onBack}>{t('account.login')}</BookButton></div> :
      <form method="post" onSubmit={submit} aria-busy={busy}>
        <label>{t('account.email')}<Input className="max-[959px]:text-base" name="email" type="email" autoComplete="email" required disabled={busy || sent} value={email} onChange={event => setEmail(event.target.value)} placeholder="name@example.com"/></label>
        {sent && <>
          <p className="account-feedback" role="status">{t('account.resetCodeSent')}</p>
          <label>{t('account.emailCode')}<span className="account-code"><Input ref={codeInput} className="max-[959px]:text-base" name="code" autoComplete="one-time-code" inputMode="numeric" pattern="[0-9]{6}" maxLength={6} required value={code} onChange={event => setCode(event.target.value.replace(/\D/g, ''))} placeholder={t('account.codePlaceholder')}/><BookButton variant="action" type="button" disabled={busy || remaining > 0} onClick={() => void request(false)}>{remaining ? `${remaining}s` : t('account.requestCode')}</BookButton></span></label>
          <label>{t('account.newPassword')}<PasswordField name="password" autoComplete="new-password" minLength={8} maxLength={128} required disabled={busy} value={password} onChange={event => setPassword(event.target.value)} placeholder={t('account.newPasswordPlaceholder')}/></label>
          <label>{t('account.confirmPassword')}<PasswordField name="confirmation" autoComplete="new-password" maxLength={128} required disabled={busy} value={confirmation} onChange={event => setConfirmation(event.target.value)} aria-invalid={confirmation.length > 0 && password !== confirmation ? true : undefined}/></label>
        </>}
        {error && <p className="account-feedback is-error" role="alert">{t(error)}</p>}
        <BookButton variant="primary" type="submit" className="account-submit" disabled={busy}>{sent ? t('account.resetPassword') : t('account.requestCode')}</BookButton>
        {sent && <BookButton variant="quiet" type="button" disabled={busy} onClick={() => {setSent(false);setRemaining(0);setCode('');setPassword('');setConfirmation('');setError(null);}}>{t('account.editDetails')}</BookButton>}
      </form>}
  </div>;
}
