import {Input} from '../../ui/input';
import {Tabs, TabsList, TabsTrigger, TabsContent} from '../../ui/tabs';
import {BookButton} from './BookButton';
import {useCallback, useEffect, useRef, useState, type ComponentType, type FormEvent} from 'react';
import {useSearchParams} from 'react-router';
import backgroundUrl from '../../../public/assets/launcher-background.webp?url';
import './account-page.css';
import {useLocale, type UiMessageKey} from '../../i18n';

export interface AccountCaptchaProps {
  onVerify(token: string): void;
  onExpire(): void;
  onError(): void;
}

/** The provider owns widget loading and cleanup; tokens stay in memory. */
export function AccountPage({captcha: Captcha, artworkSrc = backgroundUrl}: {captcha?: ComponentType<AccountCaptchaProps>; artworkSrc?: string}) {
  const {t} = useLocale();
  const [search, setSearch] = useSearchParams();
  const registering = search.get('mode') === 'register';
  const [username, setUsername] = useState(''), [email, setEmail] = useState('');
  const [identity, setIdentity] = useState(''), [password, setPassword] = useState(''), [confirmation, setConfirmation] = useState('');
  const [showPassword, setShowPassword] = useState(false), [code, setCode] = useState('');
  const [captchaToken, setCaptchaToken] = useState('');
  const verifyCaptcha = useCallback((token: string) => setCaptchaToken(token), []);
  const clearCaptcha = useCallback(() => setCaptchaToken(''), []);
  const [codeEmail, setCodeEmail] = useState(''), [remaining, setRemaining] = useState(0);
  const [message, setMessage] = useState<UiMessageKey | null>(null), [error, setError] = useState<UiMessageKey | null>(null);
  const messageRef = useRef<HTMLParagraphElement>(null);
  const pageRef = useRef<HTMLElement>(null);
  useEffect(() => {
    const header = document.querySelector('.book-masthead');
    if (!header) return;
    const measure = () => {
      const style = getComputedStyle(header);
      const overlap = Math.max(parseFloat(style.borderBottomLeftRadius) || 0, parseFloat(style.borderBottomRightRadius) || 0);
      pageRef.current?.style.setProperty('--account-header-height', `${header.getBoundingClientRect().height}px`);
      pageRef.current?.style.setProperty('--account-header-overlap', `${overlap}px`);
    };
    const observer = new ResizeObserver(measure);
    observer.observe(header); measure();
    return () => observer.disconnect();
  }, []);
  useEffect(() => {
    if (!remaining) return;
    const timer = window.setTimeout(() => setRemaining(value => Math.max(0, value - 1)), 1000);
    return () => window.clearTimeout(timer);
  }, [remaining]);
  // biome-ignore lint/correctness/useExhaustiveDependencies: Switching login/register retires credentials and verification state from the previous form.
  useEffect(() => {
    setPassword(''); setConfirmation(''); setCode(''); setCaptchaToken(''); setShowPassword(false); setMessage(null); setError(null);
  }, [registering]);
  // biome-ignore lint/correctness/useExhaustiveDependencies: A captcha token belongs to its submitted email address.
  useEffect(() => {setCaptchaToken('');}, [email]);
  useEffect(() => {if (error) messageRef.current?.focus();}, [error]);
  const sendCode = () => {
    setError(null); setMessage(null);
    if (!/^[^\s@]+@[^\s@]+\.[^\s@]+$/.test(email.trim())) {setError('account.emailInvalid'); return;}
    setCodeEmail(email.trim()); setRemaining(60); setMessage('account.demoCode');
  };
  const submit = (event: FormEvent<HTMLFormElement>) => {
    event.preventDefault(); setError(null); setMessage(null);
    if (!registering) {
      if (!identity.trim()) {setError('account.identityRequired'); return;}
      if (!password) {setError('account.passwordRequired'); return;}
      setMessage('account.loginDemo'); return;
    }
    if (!username.trim()) {setError('account.usernameRequired'); return;}
    if (password.length < 8) {setError('account.passwordLength'); return;}
    if (password !== confirmation) {setError('account.passwordMismatch'); return;}
    if (codeEmail !== email.trim() || code !== '123456') {setError('account.codeRequired'); return;}
    if (Captcha && !captchaToken) {setError('account.captchaRequired'); return;}
    setMessage('account.registerDemo');
  };
  return <Tabs value={registering ? 'register' : 'login'} onValueChange={mode => setSearch(previous => {const next = new URLSearchParams(previous); if (mode === 'register') next.set('mode', mode); else next.delete('mode'); return next;})} asChild><main ref={pageRef} className="account-page" aria-labelledby="account-title">
    <section className={`account-shell${registering ? ' is-registering' : ''}`}>
      <aside className="account-art" aria-hidden="true"><img src={artworkSrc} alt="" className="account-art-image"/></aside>
      <div className="account-content">
        <TabsList asChild><nav className="account-tabs" aria-label={t('account.actions')}>{[['login',t('account.login')],['register',t('account.register')]].map(([mode,label]) => <TabsTrigger key={mode} value={mode} aria-current={registering===(mode==='register') ? 'page' : undefined}>{label}</TabsTrigger>)}</nav></TabsList>
        <TabsContent asChild value={registering ? 'register' : 'login'}><div key={registering ? 'register' : 'login'} className="account-mode-content">
        <header><h1 id="account-title">{registering ? t('account.registerTitle') : t('account.login')}</h1></header>
        <form method="post" onSubmit={submit}>
          {registering ? <>
            <label>{t('account.username')}<Input name="username" autoComplete="username" maxLength={32} required value={username} onChange={event => setUsername(event.target.value)} placeholder={t('account.usernamePlaceholder')}/></label>
            <label>{t('account.email')}<Input name="email" type="email" autoComplete="email" required value={email} onChange={event => {setEmail(event.target.value); setCaptchaToken('');}} placeholder="name@example.com"/></label>
            <label>{t('account.emailCode')}<span className="account-code"><Input name="code" autoComplete="one-time-code" inputMode="numeric" pattern="[0-9]{6}" maxLength={6} required value={code} onChange={event => setCode(event.target.value.replace(/\D/g, ''))} placeholder={t('account.codePlaceholder')}/><BookButton variant="action" className="h-auto min-h-11 min-w-[105px] self-stretch" type="button" disabled={remaining > 0} onClick={sendCode}>{remaining ? `${remaining}s` : t('account.requestCode')}</BookButton></span></label>
          </> : <label>{t('account.identity')}<Input name="identity" autoComplete="username" required value={identity} onChange={event => setIdentity(event.target.value)} placeholder={t('account.identityPlaceholder')}/></label>}
          <label>{t('account.password')}<span className="account-password"><Input className="pr-12" aria-label={t('account.password')} name="password" type={showPassword ? 'text' : 'password'} autoComplete={registering ? 'new-password' : 'current-password'} minLength={registering ? 8 : undefined} required value={password} onChange={event => setPassword(event.target.value)} placeholder={registering ? t('account.newPasswordPlaceholder') : t('account.passwordPlaceholder')}/><BookButton variant="icon" type="button" className="account-password-toggle" aria-label={showPassword ? t('account.hidePassword') : t('account.showPassword')} aria-pressed={showPassword} onClick={() => setShowPassword(value => !value)}><svg viewBox="0 0 24 24" aria-hidden="true"><path d="M2 12s3.5-7 10-7 10 7 10 7-3.5 7-10 7S2 12 2 12Z"/><circle cx="12" cy="12" r="3"/>{showPassword && <path d="m3 3 18 18"/>}</svg></BookButton></span></label>
          {registering && <><label>{t('account.confirmPassword')}<Input name="confirmation" type={showPassword ? 'text' : 'password'} autoComplete="new-password" required value={confirmation} onChange={event => setConfirmation(event.target.value)} placeholder={t('account.confirmationPlaceholder')} aria-invalid={confirmation.length > 0 && password !== confirmation ? true : undefined}/>{confirmation.length > 0 && password !== confirmation && <small className="account-field-error">{t('account.passwordMismatch')}</small>}</label>
            <div className="account-captcha-field"><span>{t('account.verification')}</span><div className="account-captcha-slot" data-captcha-slot="">{Captcha && <Captcha key={email} onVerify={verifyCaptcha} onExpire={clearCaptcha} onError={clearCaptcha}/>}</div></div>
          </>}
          {error && <p className="account-feedback is-error" role="alert" ref={messageRef} tabIndex={-1}>{t(error)}</p>}
          {message && <p className="account-feedback" role="status">{t(message)}</p>}
          <BookButton variant="primary" type="submit" className="account-submit">{registering ? t('account.register') : t('account.login')}<span aria-hidden="true">→</span></BookButton>
        </form>
        {!registering && <><div className="account-divider"><span>{t('account.or')}</span></div><BookButton variant="action" type="button" className="account-discord" onClick={() => {setError(null); setMessage('account.discordPending');}}><DiscordIcon/>{t('account.discord')}</BookButton></>}
        <p className="account-demo-note">{t('account.preview')}</p>
        </div></TabsContent>
      </div>
    </section>
  </main></Tabs>;
}

function DiscordIcon() {return <svg viewBox="0 0 24 24" fill="currentColor" aria-hidden="true"><path d="M19.7 5.2A18 18 0 0 0 15.5 4l-.5 1a15 15 0 0 0-6 0l-.5-1a18 18 0 0 0-4.2 1.2C1.7 9 1 12.6 1.4 16.2a17 17 0 0 0 5.2 2.6l1-1.6-1.6-.8.4-.3a13 13 0 0 0 11.2 0l.4.3-1.6.8 1 1.6a17 17 0 0 0 5.2-2.6c.5-4.2-.8-7.8-2.9-11ZM8.3 14.6c-1 0-1.7-.9-1.7-2s.7-2 1.7-2 1.8.9 1.7 2c0 1.1-.7 2-1.7 2Zm7.4 0c-1 0-1.7-.9-1.7-2s.7-2 1.7-2 1.8.9 1.7 2c0 1.1-.7 2-1.7 2Z"/></svg>;}
