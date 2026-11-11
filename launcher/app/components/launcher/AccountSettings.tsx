import type {CloudSaveSync} from '../../services/cloud-save-sync';
import {useEffect, useLayoutEffect, useRef, useState, type FormEvent} from 'react';
import {Link, useLocation} from 'react-router';
import {ChevronLeft, ChevronRight, Gamepad2, ImagePlus, LoaderCircle, Plus, Save, ShieldCheck, Trash2, UserRound} from 'lucide-react';
import {Input, Textarea} from '../../ui/input';
import {Button} from '../../ui/button';
import {Toggle} from '../../ui/controls';
import {Field, FieldContent, FieldGroup, FieldLabel} from '../../ui/field';
import {animateSurface} from '../../ui/surface-motion';
import {AccountRequestError,accountRequest,mailCaptchaError} from '../../services/account-client';
import {notifyRoomProfileChange} from '../../services/account-session';
import {prepareCroppedProfileImage, prepareStoredAvatarThumbnail, validateProfileImage, ProfileImageError, type ProfileImageCrop, type ProfileImageKind} from '../../services/profile-image';
import {useLocale, type UiMessageKey} from '../../i18n';
import {AccountAvatar} from './AccountAvatar';
import {useUnsavedChanges} from '../../navigation/unsaved-changes';
import {useSurfaceLayerState} from '../../navigation/surface-layer';
import {CloudSaves} from './CloudSaves';
import type {LocalSaveReader} from '../../services/local-save-storage';
import ProfileImageCropDialog from './ProfileImageCropDialog';
import type {Profile, ProfileDisplay, ProfileReview} from './ProfilePreview';
import './account-settings.css';

type SaveProfile = (profile:Profile, display:ProfileDisplay, review?:ProfileReview) => void;
type ProfileDraft = {draft:Profile & {display:ProfileDisplay}; review:ProfileReview};

export function AccountSettings({initialProfile, initialDisplay, reviewPending, panel, onSave,readLocalSaves,cloudSync}: {
  initialProfile:Profile; initialDisplay:ProfileDisplay; panel:string;
  reviewPending?:boolean; onSave:SaveProfile;
  readLocalSaves?:LocalSaveReader;cloudSync?:CloudSaveSync|null;
}) {
  const {t} = useLocale();
  const location = useLocation();
  const editing = panel === '#edit';
  const gameSettings = panel === '#game-settings';
  const detail = panel !== '#settings';
  const content = useRef<HTMLDivElement>(null);
  // biome-ignore lint/correctness/useExhaustiveDependencies: Panel navigation resets scroll and animates the new content.
  useLayoutEffect(() => {
    const node = content.current;
    if (!node) return;
    window.scrollTo(0,0);
    const animation = animateSurface(node, 'enter', {kind:'content'});
    return () => animation?.cancel();
  },[panel]);
  const address = (hash:string) => ({pathname:location.pathname,search:location.search,hash});
  return <main className="account-settings" data-detail={detail} aria-label={t('profile.settings')}>
    <aside className="account-settings-nav">
      <h1>{t('profile.settings')}</h1>
      <nav aria-label={t('profile.settingsNavigation')}>
        <Link to={address('#security')} aria-current={!editing&&!gameSettings?'page':undefined}><ShieldCheck aria-hidden="true"/><span>{t('profile.security')}</span><ChevronRight aria-hidden="true"/></Link>
        <Link to={address('#edit')} aria-current={editing?'page':undefined}><UserRound aria-hidden="true"/><span>{t('profile.personalSettings')}</span><ChevronRight aria-hidden="true"/></Link>
        <Link to={address('#game-settings')} aria-current={gameSettings?'page':undefined}><Gamepad2 aria-hidden="true"/><span>{t('room.gameSettingsGroup')}</span><ChevronRight aria-hidden="true"/></Link>
      </nav>
    </aside>
    <div className="account-settings-content" ref={content}>
      <Link className="account-settings-back" to={address('#settings')}><ChevronLeft aria-hidden="true"/>{t('profile.settings')}</Link>
      {editing ? reviewPending === undefined
        ? <ProfileSettings initialProfile={initialProfile} initialDisplay={initialDisplay} onSave={onSave}/>
        : <ProfileDraftSettings pending={reviewPending} onSave={onSave}/>
        : gameSettings ? <CloudSaves readLocalSaves={readLocalSaves} sync={cloudSync}/> : <SecuritySettings/>}
    </div>
  </main>;
}

function ProfileDraftSettings({pending,onSave}:{pending:boolean;onSave:SaveProfile}) {
  const {t} = useLocale();
  const [draft,setDraft] = useState<ProfileDraft>();
  const [failed,setFailed] = useState(false);
  const [attempt,setAttempt] = useState(0);
  const loaded = useRef(false);
  // biome-ignore lint/correctness/useExhaustiveDependencies: Retry reloads the private draft for this editor.
  useEffect(() => {
    if (pending && loaded.current) return;
    const controller = new AbortController();
    setFailed(false);
    void accountRequest<ProfileDraft>('v1/me/profile',undefined,{signal:controller.signal}).then(next => {
      if (controller.signal.aborted) return;
      loaded.current = true;
      setDraft(current => current ? {...current,review:next.review} : next);
    }).catch(() => {if (!controller.signal.aborted) setFailed(true);});
    return () => controller.abort();
  }, [pending,attempt]);
  if (!draft) return <section aria-busy={!failed}>{failed && <p role="alert">{t('account.requestFailed')}{' '}
    <Button type="button" variant="outline" onClick={() => setAttempt(value=>value+1)}>{t('action.retry')}</Button></p>}</section>;
  const {display,...profile} = draft.draft;
  return <ProfileSettings initialProfile={profile} initialDisplay={display} review={draft.review}
    onSave={(next,visibility,review) => {
      setDraft({draft:{...next,display:visibility},review:review ?? {}});
      onSave(next,visibility,review);
    }}/>;
}

function ProfileSettings({initialProfile,initialDisplay,review,onSave}: {
  initialProfile:Profile;initialDisplay:ProfileDisplay;review?:ProfileReview;onSave:SaveProfile;
}) {
  const {t} = useLocale();
  const [profile,setProfile] = useState(initialProfile);
  const [roomThumbnail,setRoomThumbnail] = useState<string>();
  const [visible,setVisible] = useState(initialDisplay.visible);
  const [busy,setBusy] = useState(false);
  const [processing,setProcessing] = useState<Partial<Record<ProfileImageKind,boolean>>>({});
  const uploadTasks = useRef<Partial<Record<ProfileImageKind,AbortController>>>({});
  const [cropRequest,setCropRequest]=useState<{file:File;kind:ProfileImageKind}>();
  const [cropOpen,setCropOpen]=useSurfaceLayerState<boolean>('profile-image-crop',false);
  const processingImages = !!processing.avatar || !!processing.cover;
  const [error,setError] = useState<UiMessageKey>();
  const [saved,setSaved] = useState(false);
  const avatarInput = useRef<HTMLInputElement>(null), coverInput = useRef<HTMLInputElement>(null);
  const nextId = useRef(0);
  const [linkIds,setLinkIds] = useState(() => initialProfile.links.map(() => nextId.current++));
  useEffect(() => () => {
    for (const task of Object.values(uploadTasks.current)) task?.abort();
    uploadTasks.current = {};
  }, []);
  const dirty = cropOpen || processingImages || profile.name !== initialProfile.name || profile.bio !== initialProfile.bio ||
    profile.avatar !== initialProfile.avatar || !!roomThumbnail || profile.cover !== initialProfile.cover ||
    Object.keys(visible).some(key=>visible[key as keyof typeof visible]!==initialDisplay.visible[key as keyof typeof visible]) || profile.links.length !== initialProfile.links.length ||
    profile.links.some((link,index) => link.label !== initialProfile.links[index]?.label || link.url !== initialProfile.links[index]?.url);
  useUnsavedChanges(dirty, () => {
    for (const task of Object.values(uploadTasks.current)) task?.abort();
    uploadTasks.current = {};setProcessing({});setError(undefined);
    setProfile(initialProfile);setRoomThumbnail(undefined);setVisible(initialDisplay.visible);setSaved(false);setCropOpen(false);setCropRequest(undefined);
    setLinkIds(initialProfile.links.map(() => nextId.current++));
  });
  const change = (next:Profile) => {setProfile(next);setSaved(false);setError(undefined);};
  const chooseImage = (file:File|undefined,key:ProfileImageKind) => {
    if(!file)return;
    try {validateProfileImage(file);} catch(error) {setError(error instanceof ProfileImageError?`profile.image.${error.code}`:'profile.image.readFailed');return;}
    uploadTasks.current[key]?.abort();delete uploadTasks.current[key];setProcessing(current=>({...current,[key]:false}));
    setError(undefined);setCropRequest({file,kind:key});setCropOpen(true);
  };
  const stageImage = async (file:File,key:ProfileImageKind,crop:ProfileImageCrop,thumbnail?:string) => {
    uploadTasks.current[key]?.abort();
    const task = new AbortController();
    uploadTasks.current[key] = task;
    setProcessing(current=>({...current,[key]:true}));setSaved(false);setError(undefined);
    try {
      const image = await prepareCroppedProfileImage(file,key,crop,task.signal);
      if (task.signal.aborted || uploadTasks.current[key] !== task) return;
      setProfile(current=>({...current,[key]:image}));setError(undefined);
      if (key === 'avatar') setRoomThumbnail(thumbnail);
    } catch (error) {
      if (!task.signal.aborted && uploadTasks.current[key] === task)
        setError(error instanceof ProfileImageError ? `profile.image.${error.code}` : 'profile.image.readFailed');
    } finally {
      if (uploadTasks.current[key] === task) {
        delete uploadTasks.current[key];
        setProcessing(current=>({...current,[key]:false}));
      }
    }
  };
  const removeImage = (key:ProfileImageKind) => {
    uploadTasks.current[key]?.abort();delete uploadTasks.current[key];
    setProcessing(current=>({...current,[key]:false}));
    setProfile(current=>({...current,[key]:''}));setSaved(false);setError(undefined);
    if (key === 'avatar') setRoomThumbnail(undefined);
  };
  async function save(event:FormEvent) {
    event.preventDefault();
    if (busy || processingImages || cropOpen) return;
    if ([...profile.name.trim()].length > 12) {setError('account.nicknameLength');return;}
    const links = profile.links.map(link => ({label:link.label.trim(),url:link.url.trim()}));
    if (links.some(link => {
      try {return !link.label || !['https:','http:'].includes(new URL(link.url).protocol);} catch {return true;}
    })) {setError('profile.links.invalid');return;}
    setBusy(true);setError(undefined);setSaved(false);
    const {roomAvatarReady, ...fields} = profile;
    const next = {...fields,name:profile.name.trim(),links};
    const display = {...initialDisplay,visible};
    try {
      let avatarThumbnail = roomThumbnail ?? (next.avatar && roomAvatarReady === false
        ? await prepareStoredAvatarThumbnail(next.avatar).catch(() => undefined)
        : undefined);
      const saveDetails = {...next,display};
      let result: {review?:ProfileReview} | undefined;
      try {
        result = await accountRequest<typeof result>('v1/me/profile',{...saveDetails,avatarThumbnail});
      } catch (error) {
        if (roomThumbnail || !avatarThumbnail || !(error instanceof AccountRequestError && error.code === 'SAVE_QUOTA_EXCEEDED')) throw error;
        avatarThumbnail = undefined;
        result = await accountRequest<typeof result>('v1/me/profile',saveDetails);
      }
      const savedProfile = {...next,roomAvatarReady:!!next.avatar && (!!avatarThumbnail || !!roomAvatarReady)};
      setProfile(current => current === profile ? savedProfile : current);
      setRoomThumbnail(undefined);
      onSave(savedProfile,display,result?.review);setSaved(true);
      window.dispatchEvent(new Event('account-profile-change'));
      if (next.name !== initialProfile.name || next.avatar !== initialProfile.avatar || avatarThumbnail ||
          visible.records !== initialDisplay.visible.records) notifyRoomProfileChange();
    } catch(error) {setError(error instanceof AccountRequestError && error.code==='CONTENT_REJECTED'?'profile.contentRejected':error instanceof AccountRequestError && error.code==='REVIEW_UNAVAILABLE'?'profile.reviewUnavailable':error instanceof AccountRequestError && error.code==='SAVE_QUOTA_EXCEEDED'?'cloudSave.quota':'account.requestFailed');}
    finally {setBusy(false);}
  }
  const reviewStates = Object.values(review ?? {});
  const reviewMessage = reviewStates.includes('rejected') ? 'profile.contentRejected'
    : reviewStates.includes('pending') ? 'profile.reviewPending' : undefined;
  return <section aria-labelledby="profile-settings-title">
    {cropRequest&&<ProfileImageCropDialog open={cropOpen} file={cropRequest.file} kind={cropRequest.kind}
      onClose={()=>setCropOpen(false)}
      onClosed={()=>setCropRequest(current=>current===cropRequest?undefined:current)}
      onConfirm={(crop,thumbnail)=>{setCropOpen(false);void stageImage(cropRequest.file,cropRequest.kind,crop,thumbnail);}}/>}
    <header className="account-settings-heading"><h2 id="profile-settings-title">{t('profile.personalSettings')}</h2></header>
    <form onSubmit={save}>
      <div className="profile-settings-editor">
        <div className="settings-artwork">
          <div className="settings-profile-preview" aria-hidden="true">
            <div className="settings-preview-cover">{profile.cover && <img src={profile.cover} alt=""/>}</div>
            <div className="settings-preview-identity"><AccountAvatar src={profile.avatar}/><strong>{profile.name || t('profile.name.missing')}</strong>
              {profile.bio && <p>{profile.bio}</p>}
            </div>
          </div>
          <FieldGroup>
            <Field orientation="horizontal" className="settings-artwork-upload"><FieldLabel htmlFor="settings-avatar">{t('profile.avatar')}</FieldLabel><FieldContent>
              <div className="settings-media-actions">
                <Button variant="outline" size="sm" type="button" disabled={busy} aria-busy={!!processing.avatar} onClick={()=>avatarInput.current?.click()}>{processing.avatar ? <LoaderCircle className="animate-spin motion-reduce:animate-none" aria-hidden="true"/> : <ImagePlus aria-hidden="true"/>}{t('profile.image.choose')}</Button>
                <input ref={avatarInput} className="sr-only" id="settings-avatar" aria-label={t('profile.avatar')} type="file" accept="image/png,image/jpeg,image/webp" disabled={busy} onChange={event=>{const file=event.target.files?.[0];event.target.value='';chooseImage(file,'avatar');}}/>
                {(profile.avatar || processing.avatar) && <Button variant="ghost" size="icon-sm" type="button" disabled={busy} aria-label={t('profile.avatar')+' | '+t('profile.remove')} onClick={()=>removeImage('avatar')}><Trash2 aria-hidden="true"/></Button>}
              </div>
            </FieldContent></Field>
            <Field orientation="horizontal" className="settings-artwork-upload"><FieldLabel htmlFor="settings-cover">{t('profile.cover')}</FieldLabel><FieldContent>
              <div className="settings-media-actions">
                <Button variant="outline" size="sm" type="button" disabled={busy} aria-busy={!!processing.cover} onClick={()=>coverInput.current?.click()}>{processing.cover ? <LoaderCircle className="animate-spin motion-reduce:animate-none" aria-hidden="true"/> : <ImagePlus aria-hidden="true"/>}{t('profile.image.choose')}</Button>
                <input ref={coverInput} className="sr-only" id="settings-cover" aria-label={t('profile.cover')} type="file" accept="image/png,image/jpeg,image/webp" disabled={busy} onChange={event=>{const file=event.target.files?.[0];event.target.value='';chooseImage(file,'cover');}}/>
                {(profile.cover || processing.cover) && <Button variant="ghost" size="icon-sm" type="button" disabled={busy} aria-label={t('profile.cover')+' | '+t('profile.remove')} onClick={()=>removeImage('cover')}><Trash2 aria-hidden="true"/></Button>}
              </div>
            </FieldContent></Field>
          </FieldGroup>
          <small className="settings-image-hint">{t('profile.image.formats')}</small>
          {processingImages && <span className="sr-only" role="status">{t('profile.image.processing')}</span>}
        </div>
        <div className="settings-text-fields">
          <FieldGroup>
            <Field><FieldLabel htmlFor="settings-name">{t('profile.name')}</FieldLabel><FieldContent>
              <Input id="settings-name" autoComplete="nickname" required maxLength={24} value={profile.name} onChange={event=>change({...profile,name:[...event.target.value].slice(0,12).join('')})} placeholder={t('profile.name.placeholder')}/>
            </FieldContent></Field>
            <Field><FieldLabel htmlFor="settings-bio">{t('profile.bio')}</FieldLabel><FieldContent>
              <Textarea id="settings-bio" maxLength={500} rows={4} value={profile.bio} onChange={event=>change({...profile,bio:event.target.value})} placeholder={t('profile.bio.placeholder')}/>
              <small className="settings-character-count">{profile.bio.length} / 500</small>
            </FieldContent></Field>
            <Field aria-labelledby="settings-links-label"><span id="settings-links-label" data-slot="field-label" className="ui-field-label">{t('profile.links')}</span><FieldContent>
              <div className="settings-links">{profile.links.map((link,index)=><div className="settings-link" key={linkIds[index]}>
                <Input aria-label={t('profile.links.name')} maxLength={40} value={link.label} placeholder={t('profile.links.name')} onChange={event=>change({...profile,links:profile.links.map((item,i)=>i===index?{...item,label:event.target.value}:item)})}/>
                <Input aria-label={t('profile.links.address')} type="url" value={link.url} placeholder="https://" onChange={event=>change({...profile,links:profile.links.map((item,i)=>i===index?{...item,url:event.target.value}:item)})}/>
                <Button variant="ghost" size="icon-sm" type="button" aria-label={t('profile.links.delete',{number:index+1})} onClick={()=>{change({...profile,links:profile.links.filter((_,i)=>i!==index)});setLinkIds(linkIds.filter((_,i)=>i!==index));}}><Trash2 aria-hidden="true"/></Button>
              </div>)}</div>
              <Button variant="outline" size="sm" type="button" disabled={profile.links.length>=12} onClick={()=>{change({...profile,links:[...profile.links,{label:'',url:''}]});setLinkIds([...linkIds,nextId.current++]);}}><Plus aria-hidden="true"/>{t('profile.links.add')}</Button>
            </FieldContent></Field>
          </FieldGroup>
          <section className="settings-section" aria-labelledby="settings-privacy-title">
            <h3 id="settings-privacy-title">{t('profile.displayTab')}</h3>
            {(['career','records','friends'] as const).map(key=><Field key={key} orientation="horizontal" className="settings-switch-row"><FieldLabel htmlFor={'settings-'+key}>{t(key==='friends'?'profile.friendsVisibility':key==='career'?'profile.careerVisibility':'profile.recordsVisibility')}</FieldLabel><FieldContent>
              <Toggle id={'settings-'+key} checked={visible[key]} onCheckedChange={value=>{setVisible(current=>({...current,[key]:value}));setSaved(false);}}/>
            </FieldContent></Field>)}
          </section>
        </div>
      </div>
      <div className="settings-form-actions settings-profile-actions">
        {error && <span role="alert">{t(error)}</span>}
        {!error && (reviewMessage || saved && !dirty) && <span role="status">{t(reviewMessage ?? 'profile.saved')}</span>}
        <Button type="submit" className="settings-save-profile" aria-busy={busy} disabled={busy || processingImages}>{busy?<LoaderCircle className="animate-spin motion-reduce:animate-none" aria-hidden="true"/>:<Save aria-hidden="true"/>}{t('profile.save')}</Button>
      </div>
    </form>
  </section>;
}

function SecuritySettings() {
  const {t} = useLocale();
  const [email,setEmail] = useState(''), [otp,setOtp] = useState(''), [sentTo,setSentTo] = useState('');
  const [current,setCurrent] = useState(''), [password,setPassword] = useState(''), [confirmation,setConfirmation] = useState('');
  const [feedback,setFeedback] = useState<UiMessageKey>(), [failure,setFailure] = useState<UiMessageKey>(), [pending,setPending] = useState(false);
  const [remaining,setRemaining] = useState(0);
  useEffect(()=>{
    if (!remaining) return;
    const timer=window.setTimeout(()=>setRemaining(remaining-1),1000);
    return ()=>window.clearTimeout(timer);
  },[remaining]);
  async function send(path:string,body:Record<string,unknown>,message:UiMessageKey) {
    if (pending) return false;
    setFailure(undefined);setFeedback(undefined);setPending(true);
    try {
      await accountRequest('auth/'+path,body,{emailCaptcha:path==='email-otp/request-email-change'});
      setFeedback(message);return true;
    } catch (cause) {
      if (!(cause instanceof DOMException && cause.name === 'AbortError')) setFailure(mailCaptchaError(cause) ?? 'account.requestFailed');
      return false;
    }
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
    {failure && <p className="settings-feedback" role="alert">{t(failure)}</p>}
    {feedback && <p className="settings-feedback" role="status">{t(feedback)}</p>}
  </section>;
}
