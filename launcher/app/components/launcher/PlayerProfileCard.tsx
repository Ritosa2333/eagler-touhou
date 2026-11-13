import {ArrowUpRight} from 'lucide-react';
import {useEffect, useState} from 'react';
import {z} from 'zod';
import {withUiLanguage} from '../../../src/launcher/i18n.mts';
import {useLocale} from '../../i18n';
import {normalizeProfileDisplay} from '../../models/profile-display';
import {th09CharacterName, th09CharacterRating} from '../../models/th09-character-ratings';
import {accountRequest} from '../../services/account-client';
import {Button} from '../../ui/button';
import {PlayerCardPopover, type PlayerCardTrigger} from './PlayerCardPopover';
import {AccountAvatar} from './AccountAvatar';
import {ProfileCover} from './ProfileCover';
import {Th09CharacterPortrait} from './Th09CharacterPortrait';
import cover from './book-covers/th09.webp?url';
import './player-profile-card.css';

const dataImage = /^data:image\/(?:webp|png|jpeg);base64,/;
const publicProfile = z.object({nickname:z.string(), details:z.object({
  avatar:z.string().optional(), cover:z.string().optional(), display:z.unknown(),
}).nullish()});
const score = z.object({rating:z.number().int(), games:z.number().int().nonnegative()});
const scoreSnapshot = z.object({players:z.record(z.string(),score)});
const versusScores = z.object({characterRatings:z.array(th09CharacterRating).optional(),
  characters:z.array(z.object({character:z.number().int().min(0).max(15),games:z.number().int().positive()})).optional()});
type Summary = {total?: z.infer<typeof score>; characters: z.infer<typeof th09CharacterRating>[]};

function useRatingSummary(username: string | undefined, enabled: boolean) {
  const [result, setResult] = useState<{username: string; summary: Summary}>();
  useEffect(() => {
    if (!username || !enabled) return;
    const controller = new AbortController();
    const options = {signal:controller.signal, anonymous:true};
    void Promise.allSettled([
      accountRequest('v1/th09/ratings', undefined, options),
      accountRequest('v1/profiles/'+encodeURIComponent(username)+'/th09-duels', undefined, options),
    ]).then(([overall, versus]) => {
      if (controller.signal.aborted) return;
      const snapshot = overall.status === 'fulfilled' ? scoreSnapshot.safeParse(overall.value).data : undefined;
      const detail = versus.status === 'fulfilled' ? versusScores.safeParse(versus.value).data : undefined;
      const scores = detail?.characterRatings ?? [];
      const byCharacter = new Map(scores.map(row=>[row.character,row]));
      const characters = [...(detail?.characters ?? scores)].sort((a,b)=>b.games-a.games || a.character-b.character)
        .flatMap(item=>byCharacter.get(item.character) ?? []).slice(0,3);
      const total = snapshot && Object.hasOwn(snapshot.players,username) ? snapshot.players[username] : undefined;
      setResult({username,summary:{total,characters}});
    });
    return () => controller.abort();
  }, [username,enabled]);
  return enabled && result?.username === username ? result?.summary : undefined;
}

export function PlayerProfileCard({username,name,avatar,cover:banner,showRating=true}: {
  username?: string; name: string; avatar?: string; cover?: string; showRating?: boolean;
}) {
  const {t,locale} = useLocale();
  const rating = useRatingSummary(username,showRating);
  const number = new Intl.NumberFormat(locale);
  const characterScores = rating?.characters ?? [];
  return <article className="player-profile-card" aria-label={name}>
    <header className="player-profile-card-identity">
      <ProfileCover className="player-profile-card-cover" src={banner && dataImage.test(banner) ? banner : undefined}/>
      <AccountAvatar src={avatar}/>
      <div className="player-profile-card-name"><h3>{name}</h3>{username && <span>@{username}</span>}</div>
    </header>
    <div className="player-profile-card-body">
      {(rating?.total || characterScores.length > 0) && <section className="player-profile-card-scores" aria-label={t('guide.th09')}>
        <div className="player-profile-card-total">
          <img src={cover} alt="" width={24} height={24}/>
          <h4>{t('guide.th09')} <small>WHR</small></h4>
          {rating?.total && <strong>{number.format(rating.total.rating)}</strong>}
        </div>
        {characterScores.length > 0 && <ul className="player-profile-card-characters">
          {characterScores.map(item=><li key={item.character}>
            <Th09CharacterPortrait character={item.character}/>
            <span>{th09CharacterName(item.character,t)}</span><strong>{number.format(item.rating)}</strong>
          </li>)}
        </ul>}
      </section>}
      {username && <footer><Button variant="ghost" size="sm" asChild>
        <a href={withUiLanguage('/u/'+encodeURIComponent(username),locale)} target="_blank" rel="noopener">
          {t('profile.openHome')}<ArrowUpRight aria-hidden="true"/>
        </a>
      </Button></footer>}
    </div>
  </article>;
}

function PlayerCardDetails({username,name,avatar}: {username:string;name:string;avatar?:string}) {
  const [profile, setProfile] = useState<z.infer<typeof publicProfile>>();
  useEffect(() => {
    const controller = new AbortController();
    void accountRequest('v1/profiles/'+encodeURIComponent(username),undefined,{signal:controller.signal,anonymous:true})
      .then(value=>{if (!controller.signal.aborted) setProfile(publicProfile.parse(value));}).catch(()=>{});
    return () => controller.abort();
  }, [username]);
  return <PlayerProfileCard username={username} name={profile?.nickname || name} cover={profile?.details?.cover}
    avatar={profile?.details?.avatar || avatar} showRating={!!profile && normalizeProfileDisplay(profile.details?.display).visible.versus}/>;
}

export function PlayerAvatarCard({username,name,avatar,children}: {
  username:string; name:string; avatar?:string; children: PlayerCardTrigger;
}) {
  return <PlayerCardPopover trigger={children} label={name}>
    <PlayerCardDetails key={username} username={username} name={name} avatar={avatar}/>
  </PlayerCardPopover>;
}
