import {loginDestination, notifyAccountChange, useAccountSession} from '../../services/account-session';
import {accountRequest, AccountRequestError, mailCaptchaError} from '../../services/account-client';
import {Input} from '../../ui/input';
import {Button} from '../../ui/button';
import {FormCallout} from '../../ui/form-callout';
import {FieldHelp} from '../../ui/field-help';
import {Skeleton} from '../../ui/skeleton';
import {BookButton} from './BookButton';
import {useEffect, useLayoutEffect, useRef, useState, type FormEvent} from 'react';
import {PasswordField} from './PasswordField';
import {PasswordResetForm} from './PasswordResetForm';
import {useSearchParams} from 'react-router';
import backgroundUrl from '../../../public/assets/launcher-background.webp?url';
import './account-page.css';
import {useLocale, type UiMessageKey} from '../../i18n';

type RegistrationChallenge = {challengeId: string; expiresAt: number};
type AccountMode = 'login' | 'register' | 'reset';

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
  const entryInput = useRef<HTMLInputElement>(null);
  const codeInput = useRef<HTMLInputElement>(null);
  const focusMode = useRef<AccountMode | null>(null);
  const activeMode = resetting ? 'reset' : registering ? 'register' : 'login';
  const changeMode = (mode: AccountMode, keyboard: boolean) => {
    focusMode.current = keyboard ? mode : null;
    setSearch(previous => {
      const next = new URLSearchParams(previous);
      if (mode === 'login') next.delete('mode');
      else next.set('mode', mode);
      return next;
    });
  };
  useLayoutEffect(() => {
    if (focusMode.current !== activeMode) return;
    focusMode.current = null;
    (entryInput.current?.disabled ? codeInput.current : entryInput.current)?.focus({preventScroll: true});
  }, [activeMode]);
  const [username, setUsername] = useState(''), [email, setEmail] = useState('');
  const [identity, setIdentity] = useState(''), [loginPassword, setLoginPassword] = useState(''), [registrationPassword, setRegistrationPassword] = useState(''), [confirmation, setConfirmation] = useState('');
  const [nickname, setNickname] = useState(''), [busy, setBusy] = useState(false), [apiError, setApiError] = useState<UiMessageKey | null>(null);
  const [challenge, setChallenge] = useState<RegistrationChallenge | null>(null);
  const registrationLocked = challenge !== null;
  const [code, setCode] = useState('');
  useEffect(() => {if (registrationLocked) codeInput.current?.focus();}, [registrationLocked]);
  const [remaining, setRemaining] = useState(0), [registrationExpired, setRegistrationExpired] = useState(false);
  const [message, setMessage] = useState<UiMessageKey | null>(null), [error, setError] = useState<UiMessageKey | null>(null);
  const messageRef = useRef<HTMLDivElement>(null);
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
    const captchaError = mailCaptchaError(cause);
    if (captchaError) return captchaError;
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
        }, {emailCaptcha: true});
        if (!Number.isFinite(result?.expiresAt)) throw new AccountRequestError('ACCOUNT_RESPONSE_INVALID');
        setChallenge({...challenge, expiresAt: result.expiresAt});
      } else {
        const result = await accountRequest<RegistrationChallenge>('account/register/start', {
          email: codeTarget, username: username.trim(), name: nickname.trim(), password: registrationPassword,
        }, {emailCaptcha: true});
        if (typeof result?.challengeId !== 'string' || !result.challengeId || !Number.isFinite(result.expiresAt))
          throw new AccountRequestError('ACCOUNT_RESPONSE_INVALID');
        setChallenge(result);
      }
      setRegistrationExpired(false); setRemaining(60); setMessage('account.codeSent');
    } catch (cause) {
      if (cause instanceof DOMException && cause.name === 'AbortError') return;
      const expired = cause instanceof AccountRequestError && cause.status === 410;
      setRegistrationExpired(expired);
      setApiError(expired ? null : errorMessage(cause, registrationLocked ? 'resend' : 'start'));
    }
    finally {setBusy(false);}
  };
  const submit = async (event: FormEvent<HTMLFormElement>) => {
    event.preventDefault();
    if (busy) return;
    if (registering && !registrationLocked) {await sendCode(); return;}
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
      if (registering && cause instanceof AccountRequestError && cause.code === 'CONTENT_REJECTED') {
        setChallenge(null); setRemaining(0); setCode('');
      }
      setApiError(expired ? null : errorMessage(cause, registering ? 'verify' : 'login'));
    }
    finally {setBusy(false);}
  };
  return <div className="account-content">
        {resetting ? <PasswordResetForm emailInputRef={entryInput} onBack={keyboard => changeMode('login', keyboard)}/> :
        <div key={activeMode} className="account-mode-content">
        <header><h1 id="account-title">{registering ? t('account.registerTitle') : t('account.login')}</h1></header>
        <form method="post" onSubmit={submit} aria-busy={busy}>
          {registering ? <>
            <div className="grid min-w-0 gap-2">
              <div className="flex items-center gap-1">
                <label htmlFor="account-username">{t('account.username')}</label>
                <FieldHelp id="account-username-help" label={t('account.username')}>{t('account.usernameHelp')}</FieldHelp>
              </div>
              <Input id="account-username" aria-describedby="account-username-help" ref={entryInput} className="max-[959px]:text-base" name="username" autoComplete="username" minLength={3} maxLength={12} pattern="[A-Za-z0-9_.]{3,12}" required disabled={busy || registrationLocked} value={username} onChange={event => setUsername(event.target.value)}/>
            </div>
            <div className="grid min-w-0 gap-2">
              <div className="flex items-center gap-1">
                <label htmlFor="account-nickname">{t('account.nickname')}</label>
                <FieldHelp id="account-nickname-help" label={t('account.nickname')}>{t('account.nicknameHelp')}</FieldHelp>
              </div>
              <Input id="account-nickname" aria-describedby="account-nickname-help" className="max-[959px]:text-base" name="nickname" required maxLength={24} disabled={busy || registrationLocked} value={nickname} onChange={event => setNickname([...event.target.value].slice(0,12).join(''))}/>
            </div>
            <label>{t('account.email')}<Input className="max-[959px]:text-base" name="email" type="email" autoComplete="email" required disabled={busy || registrationLocked} value={email} onChange={event => setEmail(event.target.value)} placeholder="name@example.com"/></label>
          </> : <label>{t('account.identity')}<Input ref={entryInput} className="max-[959px]:text-base" name="identity" autoComplete="username" required value={identity} onChange={event => setIdentity(event.target.value)}/></label>}
          <div className="account-password-field">
            <label htmlFor="account-password">{t('account.password')}</label>
            <PasswordField id="account-password" name="password" autoComplete={registering ? 'new-password' : 'current-password'} minLength={registering ? 8 : undefined} maxLength={128} required disabled={busy || (registering && registrationLocked)} value={registering ? registrationPassword : loginPassword} onChange={event => registering ? setRegistrationPassword(event.target.value) : setLoginPassword(event.target.value)} placeholder={registering ? t('account.newPasswordPlaceholder') : undefined}/>
            {!registering && <Button variant="link" size="sm" className="account-forgot px-0 text-muted-foreground underline" disabled={busy} onClick={event => changeMode('reset', event.detail === 0)}>{t('account.forgotPassword')}</Button>}
          </div>
          {registering && <label htmlFor="account-password-confirmation">{t('account.confirmPassword')}<PasswordField id="account-password-confirmation" name="confirmation" autoComplete="new-password" maxLength={128} required disabled={busy || registrationLocked} value={confirmation} onChange={event => setConfirmation(event.target.value)} aria-invalid={confirmation.length > 0 && registrationPassword !== confirmation ? true : undefined}/>{confirmation.length > 0 && registrationPassword !== confirmation && <small className="account-field-error">{t('account.passwordMismatch')}</small>}</label>}
          {registering && registrationLocked && <div className="account-code-field">
            <label htmlFor="account-registration-code">{t('account.emailCode')}</label>
            <div className="account-code"><Input id="account-registration-code" ref={codeInput} className="max-[959px]:text-base" name="code" autoComplete="one-time-code" inputMode="numeric" pattern="[0-9]{6}" maxLength={6} required disabled={busy || registrationExpired} value={code} onChange={event => setCode(event.target.value.replace(/\D/g, ''))} placeholder={t('account.codePlaceholder')}/><BookButton variant="action" type="button" disabled={busy || registrationExpired || remaining > 0} onClick={sendCode}>{remaining ? `${remaining}s` : t('account.resendCode')}</BookButton></div>
          </div>}
          {apiError && <FormCallout type="error" role="alert">{t(apiError)}</FormCallout>}
          {error && <FormCallout type="error" role="alert" ref={messageRef} tabIndex={-1}>{t(error)}</FormCallout>}
          {registering && registrationExpired && <><FormCallout type="warning" role="status">{t('account.codeExpired')}</FormCallout><BookButton variant="action" type="button" className="account-edit-details" disabled={busy} onClick={() => {setChallenge(null); setRegistrationExpired(false); setRemaining(0); setCode(''); setApiError(null); setMessage(null);}}>{t('account.editDetails')}</BookButton></>}
          {message && <FormCallout type="success" role="status">{t(message)}</FormCallout>}
          <BookButton variant="primary" type="submit" className="account-submit" disabled={busy || registrationExpired && registering}>{t(registering ? registrationLocked ? 'account.register' : 'account.requestCode' : 'account.login')}</BookButton>
        </form>
        <Button variant="link" className="account-switch px-0 text-muted-foreground underline" disabled={busy} onClick={event => changeMode(registering ? 'login' : 'register', event.detail === 0)}>{t(registering ? 'account.backToLogin' : 'account.registerTitle')}</Button>
        </div>}
  </div>;
}
