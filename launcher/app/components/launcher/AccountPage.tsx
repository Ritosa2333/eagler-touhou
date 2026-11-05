import {loginDestination, notifyAccountChange, useAccountSession} from '../../services/account-session';
import {accountRequest, AccountRequestError} from '../../services/account-client';
import {Input} from '../../ui/input';
import {Tabs, TabsList, TabsTrigger, TabsContent} from '../../ui/tabs';
import {Skeleton} from '../../ui/skeleton';
import {BookButton} from './BookButton';
import {useEffect, useRef, useState, type FormEvent} from 'react';
import {ArrowRight} from 'lucide-react';
import {PasswordField} from './PasswordField';
import {PasswordResetForm} from './PasswordResetForm';
import {useSearchParams} from 'react-router';
import backgroundUrl from '../../../public/assets/launcher-background.webp?url';
import './account-page.css';
import {useLocale, type UiMessageKey} from '../../i18n';

type RegistrationChallenge = {challengeId: string; expiresAt: number};

export function AccountPage(props: {artworkSrc?:string}) {
  const {t} = useLocale(), {state,retry} = useAccountSession();
  useEffect(() => {if (state.status === 'member') location.replace(loginDestination());}, [state.status]);
  return <main className="account-page" aria-busy={state.status === 'loading'}>
    <aside className="account-art" aria-hidden="true"><img src={props.artworkSrc ?? backgroundUrl} alt="" className="account-art-image"/></aside>
    <section className="account-shell">
      {state.status === 'guest' ? <AccountForm/> : state.status === 'error' ?
        <div className="account-content"><BookButton variant="action" className="account-submit" onClick={retry}>{t('account.requestFailed')}</BookButton></div> :
        <div className="account-content account-loading" aria-hidden="true">
          <Skeleton className="mb-6 h-8 w-36"/>
          <div className="grid gap-4"><Skeleton className="h-4 w-28"/><Skeleton className="h-11 w-full"/><Skeleton className="h-4 w-20"/><Skeleton className="h-11 w-full"/><Skeleton className="h-11 w-full"/></div>
        </div>}
    </section>
  </main>;
}
function AccountForm() {
  const {t} = useLocale();
  const [search, setSearch] = useSearchParams();
  const registering = search.get('mode') === 'register';
  const resetting = search.get('mode') === 'reset';
  const changeMode = (mode: string) => setSearch(previous => {const next = new URLSearchParams(previous); if (mode === 'login') next.delete('mode'); else next.set('mode', mode); return next;});
  const [username, setUsername] = useState(''), [email, setEmail] = useState('');
  const [identity, setIdentity] = useState(''), [loginPassword, setLoginPassword] = useState(''), [registrationPassword, setRegistrationPassword] = useState(''), [confirmation, setConfirmation] = useState('');
  const [nickname, setNickname] = useState(''), [busy, setBusy] = useState(false), [apiError, setApiError] = useState<UiMessageKey | null>(null);
  const [challenge, setChallenge] = useState<RegistrationChallenge | null>(null);
  const registrationLocked = challenge !== null;
  const [code, setCode] = useState('');
  const [remaining, setRemaining] = useState(0), [registrationExpired, setRegistrationExpired] = useState(false);
  const [message, setMessage] = useState<UiMessageKey | null>(null), [error, setError] = useState<UiMessageKey | null>(null);
  const messageRef = useRef<HTMLParagraphElement>(null);
  useEffect(() => {
    if (!remaining) return;
    const timer = window.setTimeout(() => setRemaining(value => Math.max(0, value - 1)), 1000);
    return () => window.clearTimeout(timer);
  }, [remaining]);
  // biome-ignore lint/correctness/useExhaustiveDependencies: Switching modes clears login credentials while a sent registration draft stays bound to its challenge.
  useEffect(() => {
    setApiError(null); setLoginPassword(''); setMessage(null); setError(null);
    if (!registrationLocked) {
      setChallenge(null); setRemaining(0); setRegistrationPassword(''); setConfirmation(''); setCode(''); setRegistrationExpired(false);
    }
  }, [registering, resetting]);
  useEffect(() => {if (error) messageRef.current?.focus();}, [error]);
  const errorMessage = (cause: unknown, operation: 'start' | 'resend' | 'verify' | 'login'): UiMessageKey => {
    const code = cause instanceof AccountRequestError ? cause.code : '';
    const status = cause instanceof AccountRequestError ? cause.status : undefined;
    if (['USERNAME_IS_ALREADY_TAKEN', 'USERNAME_ALREADY_EXISTS'].includes(code)) return 'account.usernameInUse';
    if (code === 'CONTENT_REJECTED') return 'nickname.rejected';
    if (code === 'REVIEW_UNAVAILABLE') return 'chat.reviewUnavailable';
    if (status === 409) return 'account.emailInUse';
    if (status === 410) return 'account.codeExpired';
    if (operation === 'login' && status === 401) return 'account.credentialsInvalid';
    if (status === 400 && operation === 'verify') return 'account.codeInvalid';
    if (['USER_ALREADY_EXISTS_USE_ANOTHER_EMAIL', 'USER_ALREADY_EXISTS'].includes(code)) return 'account.emailInUse';
    if (['INVALID_EMAIL_OR_PASSWORD', 'INVALID_USERNAME_OR_PASSWORD'].includes(code)) return 'account.credentialsInvalid';
    if (['INVALID_OTP', 'INVALID_EMAIL_OTP', 'OTP_EXPIRED', 'VERIFICATION_CODE_EXPIRED'].includes(code)) return 'account.codeInvalid';
    if (code === 'TOO_MANY_REQUESTS') return 'account.tooManyRequests';
    return 'account.requestFailed';
  };
  const sendCode = async () => {
    if (busy || remaining) return;
    setError(null); setMessage(null); setApiError(null);
    const codeTarget = email.trim();
    if (!registrationLocked) {
      if (!username.trim() || !nickname.trim()) {setError('account.usernameRequired'); return;}
      if (!/^[A-Za-z0-9_.]{3,12}$/.test(username.trim())) {setError('account.usernameFormat');return;}
      if ([...nickname.trim()].length>12) {setError('account.nicknameLength');return;}
      if (!/^[^\s@]+@[^\s@]+\.[^\s@]+$/.test(codeTarget)) {setError('account.emailInvalid'); return;}
      if (registrationPassword.length < 8) {setError('account.passwordLength'); return;}
      if (registrationPassword !== confirmation) {setError('account.passwordMismatch'); return;}
    }
    if (!codeTarget) {setError('account.emailInvalid'); return;}
    setBusy(true);
    try {
      if (registrationLocked) {
        if (!challenge) throw new AccountRequestError('ACCOUNT_REQUEST_FAILED');
        const result = await accountRequest<{expiresAt: number}>('account/register/resend', {
          challengeId: challenge.challengeId, email: codeTarget, password: registrationPassword,
        });
        if (!Number.isFinite(result?.expiresAt)) throw new AccountRequestError('ACCOUNT_RESPONSE_INVALID');
        setChallenge({...challenge, expiresAt: result.expiresAt});
      } else {
        const result = await accountRequest<RegistrationChallenge>('account/register/start', {
          email: codeTarget, username: username.trim(), name: nickname.trim(), password: registrationPassword,
        });
        if (typeof result?.challengeId !== 'string' || !result.challengeId || !Number.isFinite(result.expiresAt))
          throw new AccountRequestError('ACCOUNT_RESPONSE_INVALID');
        setChallenge(result);
      }
      setRegistrationExpired(false); setRemaining(60); setMessage('account.codeSent');
    } catch (cause) {
      const expired = cause instanceof AccountRequestError && cause.status === 410;
      setRegistrationExpired(expired);
      setApiError(expired ? null : errorMessage(cause, registrationLocked ? 'resend' : 'start'));
    }
    finally {setBusy(false);}
  };
  const submit = async (event: FormEvent<HTMLFormElement>) => {
    event.preventDefault();
    if (busy) return;
    setError(null); setMessage(null); setApiError(null);
    if (registering && (!challenge || registrationExpired || code.length !== 6)) {setError('account.codeRequired'); return;}
    setBusy(true);
    try {
      if (registering && challenge) await accountRequest('account/register/verify', {
        challengeId: challenge.challengeId, email: email.trim(), otp: code, password: registrationPassword,
      });
      else if (identity.includes('@')) await accountRequest('auth/sign-in/email', {email: identity.trim(), password: loginPassword});
      else await accountRequest('auth/sign-in/username', {username: identity.trim(), password: loginPassword});
      notifyAccountChange();
      window.location.replace(loginDestination());
    } catch (cause) {
      const expired = cause instanceof AccountRequestError && cause.status === 410;
      setRegistrationExpired(registering && expired);
      setApiError(expired ? null : errorMessage(cause, registering ? 'verify' : 'login'));
    }
    finally {setBusy(false);}
  };
  return <Tabs value={registering ? 'register' : 'login'} onValueChange={changeMode} asChild><div className="account-content">
        {resetting ? <PasswordResetForm onBack={() => changeMode('login')}/> : <>
        <TabsList asChild><nav className="account-tabs" aria-label={t('account.actions')}>{[['login',t('account.login')],['register',t('account.register')]].map(([mode,label]) => <TabsTrigger key={mode} value={mode} aria-current={registering===(mode==='register') ? 'page' : undefined}>{label}</TabsTrigger>)}</nav></TabsList>
        <TabsContent asChild value={registering ? 'register' : 'login'}><div key={registering ? 'register' : 'login'} className="account-mode-content">
        <header><h1 id="account-title">{registering ? t('account.registerTitle') : t('account.login')}</h1></header>
        <form method="post" onSubmit={submit} aria-busy={busy}>
          {registering ? <>
            <label>{t('account.username')}<Input className="max-[959px]:text-base" name="username" autoComplete="username" minLength={3} maxLength={12} pattern="[A-Za-z0-9_.]{3,12}" required disabled={busy || registrationLocked} value={username} onChange={event => setUsername(event.target.value)} placeholder={t('account.usernamePlaceholder')}/></label>
            <label>{t('profile.name')}<Input className="max-[959px]:text-base" name="nickname" required maxLength={24} disabled={busy || registrationLocked} value={nickname} onChange={event => setNickname([...event.target.value].slice(0,12).join(''))}/></label>
            <label>{t('account.email')}<Input className="max-[959px]:text-base" name="email" type="email" autoComplete="email" required disabled={busy || registrationLocked} value={email} onChange={event => setEmail(event.target.value)} placeholder="name@example.com"/></label>
          </> : <label>{t('account.identity')}<Input className="max-[959px]:text-base" name="identity" autoComplete="username" required value={identity} onChange={event => setIdentity(event.target.value)} placeholder={t('account.identityPlaceholder')}/></label>}
          <label>{t('account.password')}<PasswordField key={registering ? 'register' : 'login'} aria-label={t('account.password')} name="password" autoComplete={registering ? 'new-password' : 'current-password'} minLength={registering ? 8 : undefined} maxLength={128} required disabled={busy || (registering && registrationLocked)} value={registering ? registrationPassword : loginPassword} onChange={event => registering ? setRegistrationPassword(event.target.value) : setLoginPassword(event.target.value)} placeholder={registering ? t('account.newPasswordPlaceholder') : t('account.passwordPlaceholder')}/></label>
          {!registering && <BookButton variant="quiet" className="account-forgot" type="button" onClick={() => changeMode('reset')}>{t('account.forgotPassword')}</BookButton>}
          {registering && <><label>{t('account.confirmPassword')}<PasswordField name="confirmation" autoComplete="new-password" maxLength={128} required disabled={busy || registrationLocked} value={confirmation} onChange={event => setConfirmation(event.target.value)} placeholder={t('account.confirmationPlaceholder')} aria-invalid={confirmation.length > 0 && registrationPassword !== confirmation ? true : undefined}/>{confirmation.length > 0 && registrationPassword !== confirmation && <small className="account-field-error">{t('account.passwordMismatch')}</small>}</label>
            <label>{t('account.emailCode')}<span className="account-code"><Input className="max-[959px]:text-base" name="code" autoComplete="one-time-code" inputMode="numeric" pattern="[0-9]{6}" maxLength={6} required value={code} onChange={event => setCode(event.target.value.replace(/\D/g, ''))} placeholder={t('account.codePlaceholder')}/><BookButton variant="action" type="button" disabled={busy || remaining > 0} onClick={sendCode}>{remaining ? `${remaining}s` : t('account.requestCode')}</BookButton></span></label>
          </>}
          {apiError && <p className="account-feedback is-error" role="alert">{t(apiError)}</p>}
          {error && <p className="account-feedback is-error" role="alert" ref={messageRef} tabIndex={-1}>{t(error)}</p>}
          {registering && registrationExpired && <><p className="account-feedback" role="status">{t('account.codeExpired')}</p><BookButton variant="action" type="button" className="account-edit-details" disabled={busy} onClick={() => {setChallenge(null); setRegistrationExpired(false); setRemaining(0); setCode(''); setApiError(null); setMessage(null);}}>{t('account.editDetails')}</BookButton></>}
          {message && <p className="account-feedback" role="status">{t(message)}</p>}
          <BookButton variant="primary" type="submit" className="account-submit" disabled={busy || (registering && (!registrationLocked || registrationExpired))}>{registering ? t('account.register') : t('account.login')}<ArrowRight aria-hidden="true"/></BookButton>
        </form>
        </div></TabsContent></>}
  </div></Tabs>;
}
