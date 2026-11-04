import {useEffect, useLayoutEffect, useRef, useState, type FormEvent} from 'react';
import {Link, useLocation} from 'react-router';
import {ChevronLeft, ChevronRight, Gamepad2, ShieldCheck, UserRound} from 'lucide-react';
import {Input, Textarea} from '../../ui/input';
import {Button} from '../../ui/button';
import {Toggle} from '../../ui/controls';
import {Field, FieldContent, FieldGroup, FieldLabel} from '../../ui/field';
import {animateSurface} from '../../ui/surface-motion';
import {accountRequest} from '../../services/account-client';
import {useLocale, type UiMessageKey} from '../../i18n';
import {AccountAvatar} from './AccountAvatar';
import type {Profile, ProfileDisplay} from './ProfilePreview';
import './account-settings.css';

export function AccountSettings({initialProfile, initialDisplay, panel, onSave}: {
  initialProfile:Profile; initialDisplay:ProfileDisplay; panel:string;
  onSave(profile:Profile, display:ProfileDisplay):void;
}) {
  const {t} = useLocale();
  const location = useLocation();
  const editing = panel === '#edit';
  const detail = panel !== '#settings';
  const content = useRef<HTMLDivElement>(null);
  useLayoutEffect(() => {
    const node = content.current;
    if (!node) return;
    const animation = animateSurface(node, 'enter', {kind:'content'});
    return () => animation?.cancel();
  },[panel]);
  const address = (hash:string) => ({pathname:location.pathname,search:location.search,hash});
  return <main className="account-settings" data-detail={detail} aria-label={t('profile.settings')}>
    <aside className="account-settings-nav">
      <h1>{t('profile.settings')}</h1>
      <nav aria-label={t('profile.settingsNavigation')}>
        <Link to={address('#security')} aria-current={!editing?'page':undefined}><ShieldCheck aria-hidden="true"/><span>{t('profile.security')}</span><ChevronRight aria-hidden="true"/></Link>
        <Link to={address('#edit')} aria-current={editing?'page':undefined}><UserRound aria-hidden="true"/><span>{t('profile.personalSettings')}</span><ChevronRight aria-hidden="true"/></Link>
        <span aria-disabled="true"><Gamepad2 aria-hidden="true"/><span>{t('room.gameSettingsGroup')}</span><small>{t('profile.upcoming')}</small></span>
      </nav>
    </aside>
    <div className="account-settings-content" ref={content}>
      <Link className="account-settings-back" to={address('#settings')}><ChevronLeft aria-hidden="true"/>{t('profile.settings')}</Link>
      {editing ? <ProfileSettings initialProfile={initialProfile} initialDisplay={initialDisplay} onSave={onSave}/> : <SecuritySettings/>}
    </div>
  </main>;
}

function ProfileSettings({initialProfile,initialDisplay,onSave}: {
  initialProfile:Profile;initialDisplay:ProfileDisplay;onSave(profile:Profile,display:ProfileDisplay):void;
}) {
  const {t} = useLocale();
  const [profile,setProfile] = useState(initialProfile);
  const [visible,setVisible] = useState(initialDisplay.visible.friends);
  const [busy,setBusy] = useState(false);
  const [error,setError] = useState<UiMessageKey>();
  const [saved,setSaved] = useState(false);
  const avatarInput = useRef<HTMLInputElement>(null), coverInput = useRef<HTMLInputElement>(null);
  const nextId = useRef(0);
  const [linkIds,setLinkIds] = useState(() => initialProfile.links.map(() => nextId.current++));
  const change = (next:Profile) => {setProfile(next);setSaved(false);};
  const upload = async (file:File|undefined,key:'avatar'|'cover') => {
    if (!file) return;
    if (!['image/png','image/jpeg','image/webp'].includes(file.type) || file.size>750000) {setError('profile.image.invalid');return;}
    const reader = new FileReader();
    reader.onload = () => {if (typeof reader.result === 'string') {const image=reader.result;setProfile(current=>({...current,[key]:image}));setSaved(false);setError(undefined);}};
    reader.onerror = () => setError('profile.image.readFailed');
    reader.readAsDataURL(file);
  };
  async function save(event:FormEvent) {
    event.preventDefault();
    if (busy) return;
    const links = profile.links.map(link => ({label:link.label.trim(),url:link.url.trim()}));
    if (links.some(link => {
      try {return !link.label || !['https:','http:'].includes(new URL(link.url).protocol);} catch {return true;}
    })) {setError('profile.links.invalid');return;}
    setBusy(true);setError(undefined);setSaved(false);
    const next = {...profile,name:profile.name.trim(),links};
    const display = {...initialDisplay,visible:{...initialDisplay.visible,friends:visible}};
    try {
      if (next.name !== initialProfile.name) await accountRequest('auth/update-user',{name:next.name});
      await accountRequest('v1/me/profile',{...next,display});
      onSave(next,display);setSaved(true);
      window.dispatchEvent(new Event('account-profile-change'));
    } catch {setError('account.requestFailed');}
    finally {setBusy(false);}
  }
  return <section aria-labelledby="profile-settings-title">
    <header className="account-settings-heading"><h2 id="profile-settings-title">{t('profile.personalSettings')}</h2></header>
    <form onSubmit={save}>
      <FieldGroup>
        <Field orientation="responsive"><FieldLabel htmlFor="settings-avatar">{t('profile.avatar')}</FieldLabel><FieldContent>
          <div className="settings-media-row"><AccountAvatar src={profile.avatar}/><div className="settings-media-actions">
            <Button variant="outline" size="sm" type="button" onClick={()=>avatarInput.current?.click()}>{t('profile.image.choose')}</Button>
            <input ref={avatarInput} className="sr-only" id="settings-avatar" aria-label={t('profile.avatar')} type="file" accept="image/png,image/jpeg,image/webp" onChange={event=>void upload(event.target.files?.[0],'avatar')}/>
            {profile.avatar && <Button variant="ghost" size="sm" type="button" onClick={()=>change({...profile,avatar:''})}>{t('profile.remove')}</Button>}
          </div></div>
        </FieldContent></Field>
        <Field orientation="responsive"><FieldLabel htmlFor="settings-cover">{t('profile.cover')}</FieldLabel><FieldContent>
          <div className="settings-media-row"><div className="settings-cover-preview">{profile.cover && <img src={profile.cover} alt=""/>}</div><div className="settings-media-actions">
            <Button variant="outline" size="sm" type="button" onClick={()=>coverInput.current?.click()}>{t('profile.image.choose')}</Button>
            <input ref={coverInput} className="sr-only" id="settings-cover" aria-label={t('profile.cover')} type="file" accept="image/png,image/jpeg,image/webp" onChange={event=>void upload(event.target.files?.[0],'cover')}/>
            {profile.cover && <Button variant="ghost" size="sm" type="button" onClick={()=>change({...profile,cover:''})}>{t('profile.remove')}</Button>}
          </div></div>
        </FieldContent></Field>
        <Field orientation="responsive"><FieldLabel htmlFor="settings-name">{t('profile.name')}</FieldLabel><FieldContent>
          <Input id="settings-name" autoComplete="nickname" required maxLength={32} value={profile.name} onChange={event=>change({...profile,name:event.target.value})}/>
        </FieldContent></Field>
        <Field orientation="responsive"><FieldLabel htmlFor="settings-bio">{t('profile.bio')}</FieldLabel><FieldContent>
          <Textarea id="settings-bio" maxLength={500} rows={3} value={profile.bio} onChange={event=>change({...profile,bio:event.target.value})}/>
        </FieldContent></Field>
        <Field orientation="responsive"><FieldLabel>{t('profile.links')}</FieldLabel><FieldContent>
          <div className="settings-links">{profile.links.map((link,index)=><div className="settings-link" key={linkIds[index]}>
            <Input aria-label={t('profile.links.name')} maxLength={40} value={link.label} onChange={event=>change({...profile,links:profile.links.map((item,i)=>i===index?{...item,label:event.target.value}:item)})}/>
            <Input aria-label={t('profile.links.address')} type="url" value={link.url} placeholder="https://" onChange={event=>change({...profile,links:profile.links.map((item,i)=>i===index?{...item,url:event.target.value}:item)})}/>
            <Button variant="ghost" size="sm" type="button" aria-label={t('profile.links.delete',{number:index+1})} onClick={()=>{change({...profile,links:profile.links.filter((_,i)=>i!==index)});setLinkIds(linkIds.filter((_,i)=>i!==index));}}>{t('action.delete')}</Button>
          </div>)}</div>
          <Button variant="outline" size="sm" type="button" disabled={profile.links.length>=12} onClick={()=>{change({...profile,links:[...profile.links,{label:'',url:''}]});setLinkIds([...linkIds,nextId.current++]);}}>{t('profile.links.add')}</Button>
        </FieldContent></Field>
      </FieldGroup>
      <section className="settings-section" aria-labelledby="settings-privacy-title">
        <h3 id="settings-privacy-title">{t('profile.displayTab')}</h3>
        <Field orientation="horizontal" className="settings-switch-row"><FieldLabel htmlFor="settings-friends">{t('profile.friendsVisibility')}</FieldLabel><FieldContent>
          <Toggle id="settings-friends" checked={visible} onCheckedChange={value=>{setVisible(value);setSaved(false);}}/>
        </FieldContent></Field>
      </section>
      <div className="settings-form-actions">
        <Button type="submit" disabled={busy}>{t('profile.save')}</Button>
        {error && <span role="alert">{t(error)}</span>}{saved && <span role="status">{t('profile.saved')}</span>}
      </div>
    </form>
  </section>;
}

function SecuritySettings() {
  const {t} = useLocale();
  const [email,setEmail] = useState(''), [otp,setOtp] = useState(''), [sentTo,setSentTo] = useState('');
  const [current,setCurrent] = useState(''), [password,setPassword] = useState(''), [confirmation,setConfirmation] = useState('');
  const [feedback,setFeedback] = useState<UiMessageKey>(), [failure,setFailure] = useState(false), [pending,setPending] = useState(false);
  const [remaining,setRemaining] = useState(0);
  useEffect(()=>{
    if (!remaining) return;
    const timer=window.setTimeout(()=>setRemaining(remaining-1),1000);
    return ()=>window.clearTimeout(timer);
  },[remaining]);
  async function send(path:string,body:Record<string,unknown>,message:UiMessageKey) {
    if (pending) return false;
    setFailure(false);setFeedback(undefined);setPending(true);
    try {await accountRequest('auth/'+path,body);setFeedback(message);return true;}
    catch {setFailure(true);return false;}
    finally {setPending(false);}
  }
  return <section aria-labelledby="security-settings-title">
    <header className="account-settings-heading"><h2 id="security-settings-title">{t('profile.security')}</h2></header>
    <section className="settings-section settings-section-first"><h3>{t('profile.security.changeEmail')}</h3>
      <form onSubmit={async event=>{
        event.preventDefault();
        if (sentTo!==email.trim()) {setFeedback('profile.security.emailCodeRequired');return;}
        if (await send('email-otp/change-email',{newEmail:email.trim(),otp},'profile.security.emailDone')) {setOtp('');setSentTo('');}
      }}>
        <FieldGroup>
          <Field orientation="responsive"><FieldLabel htmlFor="settings-email">{t('profile.security.newEmail')}</FieldLabel><FieldContent>
            <Input id="settings-email" type="email" autoComplete="email" required value={email} onChange={event=>{setEmail(event.target.value);setOtp('');setSentTo('');}}/>
          </FieldContent></Field>
          <Field orientation="responsive"><FieldLabel htmlFor="settings-email-code">{t('profile.security.emailCode')}</FieldLabel><FieldContent>
            <div className="settings-email-code"><Input id="settings-email-code" autoComplete="one-time-code" inputMode="numeric" pattern="[0-9]{6}" maxLength={6} required value={otp} onChange={event=>setOtp(event.target.value.replace(/\D/g,''))}/>
              <Button variant="outline" type="button" disabled={pending||remaining>0} onClick={async()=>{
                if (!/^[^\s@]+@[^\s@]+\.[^\s@]+$/.test(email.trim())) {setFeedback('profile.security.emailInvalid');return;}
                if (await send('email-otp/request-email-change',{newEmail:email.trim()},'account.codeSent')) {setSentTo(email.trim());setRemaining(60);}
              }}>{remaining?remaining+'s':t('profile.security.getCode')}</Button></div>
          </FieldContent></Field>
        </FieldGroup>
        <div className="settings-form-actions"><Button type="submit" variant="outline" disabled={pending}>{t('profile.security.changeEmail')}</Button></div>
      </form>
    </section>
    <section className="settings-section"><h3>{t('profile.security.changePassword')}</h3>
      <form onSubmit={async event=>{
        event.preventDefault();
        if (password!==confirmation) {setFeedback('profile.security.passwordMismatch');return;}
        if (await send('change-password',{currentPassword:current,newPassword:password,revokeOtherSessions:true},'profile.security.passwordDone')) {setCurrent('');setPassword('');setConfirmation('');}
      }}>
        <FieldGroup>
          <Field orientation="responsive"><FieldLabel htmlFor="settings-current-password">{t('profile.security.currentPassword')}</FieldLabel><FieldContent>
            <Input id="settings-current-password" type="password" autoComplete="current-password" required value={current} onChange={event=>setCurrent(event.target.value)}/>
          </FieldContent></Field>
          <Field orientation="responsive"><FieldLabel htmlFor="settings-new-password">{t('profile.security.newPassword')}</FieldLabel><FieldContent>
            <Input id="settings-new-password" type="password" autoComplete="new-password" minLength={8} required value={password} onChange={event=>setPassword(event.target.value)}/>
          </FieldContent></Field>
          <Field orientation="responsive"><FieldLabel htmlFor="settings-confirm-password">{t('profile.security.confirmPassword')}</FieldLabel><FieldContent>
            <Input id="settings-confirm-password" type="password" autoComplete="new-password" minLength={8} required value={confirmation} aria-invalid={confirmation.length>0&&password!==confirmation||undefined} onChange={event=>setConfirmation(event.target.value)}/>
          </FieldContent></Field>
        </FieldGroup>
        <div className="settings-form-actions"><Button type="submit" variant="outline" disabled={pending}>{t('profile.security.changePassword')}</Button></div>
      </form>
    </section>
    {failure && <p className="settings-feedback" role="alert">{t('account.requestFailed')}</p>}
    {feedback && <p className="settings-feedback" role="status">{t(feedback)}</p>}
  </section>;
}
