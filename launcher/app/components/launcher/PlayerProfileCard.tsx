import {ArrowUpRight} from 'lucide-react';
import {useEffect, useState} from 'react';
import {z} from 'zod';
import {withUiLanguage} from '../../../src/launcher/i18n.mts';
import {useLocale} from '../../i18n';
import {normalizeProfileDisplay} from '../../models/profile-display';
import {th09CharacterName, th09CharacterRating, th09RatingPoint} from '../../models/th09-character-ratings';
import {accountRequest} from '../../services/account-client';
import type {Th09RatingCard} from '../../services/th09-ratings';
import {Button} from '../../ui/button';
import {PlayerCardPopover, type PlayerCardTrigger} from './PlayerCardPopover';
import {AccountAvatar} from './AccountAvatar';
import {ProfileCover} from './ProfileCover';
import {Th09CharacterPortrait} from './Th09CharacterPortrait';
import cover from './book-covers/th09.webp?url';
import './player-profile-card.css';

const publicProfile = z.object({nickname:z.string(), media:z.object({avatar:z.string().optional(), cover:z.string().optional()}).optional(), details:z.object({
  avatar:z.string().optional(), cover:z.string().optional(), display:z.unknown(),
}).nullish()});
const characterGames = z.object({character:z.number().int().min(0).max(15),games:z.number().int().positive()});
const versusScores = z.object({
  ratingHistory:z.array(th09RatingPoint).optional(),
  characterRatings:z.array(th09CharacterRating).optional(),
  characters:z.array(characterGames).optional(),
});
type CharacterRating = z.infer<typeof th09CharacterRating>;
type RatingSummary = {total?: number; characters: CharacterRating[]};
type BoardRating = {rating:number; top_characters?: z.infer<typeof characterGames>[]; character_ratings?: CharacterRating[]};

function favoriteRatedCharacters(scores: CharacterRating[], favorites: z.infer<typeof characterGames>[]) {
  const byCharacter = new Map(scores.map(row=>[row.character,row]));
  return favorites.flatMap(row=>byCharacter.get(row.character) ?? []).slice(0,3);
}

function useRatingSummary(username: string | undefined, enabled: boolean) {
  const [result, setResult] = useState<{username: string; summary: RatingSummary}>();
  useEffect(() => {
    if (!username || !enabled) return;
    const controller = new AbortController();
    void accountRequest('v1/profiles/'+encodeURIComponent(username)+'/th09-duels', undefined,
      {signal:controller.signal, anonymous:true}).then(value => {
      const detail = versusScores.safeParse(value).data;
      if (controller.signal.aborted || !detail) return;
      const scores = detail.characterRatings ?? [];
      const favorites = [...(detail.characters ?? scores)].sort((a,b)=>b.games-a.games || a.character-b.character);
      setResult({username,summary:{
        total:detail.ratingHistory?.at(-1)?.rating,
        characters:favoriteRatedCharacters(scores,favorites),
      }});
    }).catch(()=>{});
    return () => controller.abort();
  }, [username,enabled]);
  return enabled && result?.username === username ? result?.summary : undefined;
}

export function PlayerProfileCard({username,name,avatar,highResolutionAvatar,cover:banner,showRating=true,showTotal=true,boardRating,ratingCard}: {
  username?: string; name: string; avatar?: string; highResolutionAvatar?: string; cover?: string; showRating?: boolean;
  showTotal?: boolean; boardRating?: BoardRating; ratingCard?: Th09RatingCard | null;
}) {
  const {t,locale} = useLocale();
  // The public board omits character_ratings when the player hides versus data.
  const scores = boardRating?.character_ratings;
  const favorites = scores && favoriteRatedCharacters(scores, boardRating?.top_characters ?? []);
  const needsDetails = !boardRating || !!scores && (favorites?.length ?? 0) < Math.min(3,scores.length);
  const details = useRatingSummary(username,showRating && ratingCard === undefined && needsDetails);
  const characterScores = showRating ? ratingCard?.characters.map(([character,rating])=>({character,rating}))
    ?? details?.characters ?? favorites ?? [] : [];
  const total = showRating && showTotal ? ratingCard?.rating ?? (scores ? boardRating?.rating : details?.total) : undefined;
  const number = new Intl.NumberFormat(locale);
  const [decodedAvatar, setDecodedAvatar] = useState<string>();
  useEffect(() => {
    if (!highResolutionAvatar || highResolutionAvatar === avatar) return;
    let active = true;
    const image = new Image();
    image.src = highResolutionAvatar;
    void image.decode().then(() => {
      if (active) setDecodedAvatar(highResolutionAvatar);
    }).catch(() => {});
    return () => { active = false; };
  }, [avatar, highResolutionAvatar]);
  return <article className="player-profile-card" aria-label={name}>
    <header className="player-profile-card-identity">
      <ProfileCover className="player-profile-card-cover" src={banner || undefined}/>
      <div className="player-profile-card-avatar">
        <AccountAvatar src={avatar}/>
        {decodedAvatar && decodedAvatar === highResolutionAvatar &&
          <img className="player-profile-card-avatar-detail" src={decodedAvatar} alt=""/>}
      </div>
      <div className="player-profile-card-name"><h3>{name}</h3>{username && <span>@{username}</span>}</div>
      {username && <Button variant="ghost" size="sm" className="player-profile-card-home" asChild>
        <a href={withUiLanguage('/u/'+encodeURIComponent(username),locale)} target="_blank" rel="noopener">
          {t('profile.openHome')}<ArrowUpRight aria-hidden="true"/>
        </a>
      </Button>}
    </header>
    {(total !== undefined || characterScores.length > 0) && <div className="player-profile-card-body">
      <section className="player-profile-card-scores" aria-label={t('guide.th09')}>
        <div className="player-profile-card-total">
          <img src={cover} alt="" width={24} height={24}/>
          <h4>{t('guide.th09')} <small>WHR</small></h4>
          {total !== undefined && <strong>{number.format(total)}</strong>}
        </div>
        {characterScores.length > 0 && <ul className="player-profile-card-characters">
          {characterScores.map(item=><li key={item.character}>
            <Th09CharacterPortrait character={item.character}/>
            <span>{th09CharacterName(item.character,t)}</span><strong>{number.format(item.rating)}</strong>
          </li>)}
        </ul>}
      </section>
    </div>}
  </article>;
}

function PlayerCardDetails({username,name,avatar,boardRating,ratingCard}: {
  username:string;name:string;avatar?:string;boardRating?:BoardRating;ratingCard?:Th09RatingCard | null;
}) {
  const [profile, setProfile] = useState<z.infer<typeof publicProfile>>();
  useEffect(() => {
    const controller = new AbortController();
    void accountRequest('v1/profiles/'+encodeURIComponent(username),undefined,{query:new URLSearchParams({media:'urls'}),signal:controller.signal,anonymous:true})
      .then(value=>{if (!controller.signal.aborted) setProfile(publicProfile.parse(value));}).catch(()=>{});
    return () => controller.abort();
  }, [username]);
  const visibility = profile && normalizeProfileDisplay(profile.details?.display).visible;
  return <PlayerProfileCard username={username} name={profile?.nickname || name} cover={profile?.media?.cover ?? profile?.details?.cover}
    avatar={avatar} highResolutionAvatar={profile?.media?.avatar ?? profile?.details?.avatar} boardRating={boardRating} ratingCard={ratingCard}
    showRating={visibility?.versus ?? true} showTotal={visibility?.records ?? !!(boardRating || ratingCard)}/>;
}

export function PlayerAvatarCard({username,name,avatar,boardRating,ratingCard,children}: {
  username:string; name:string; avatar?:string; boardRating?:BoardRating; ratingCard?:Th09RatingCard | null; children: PlayerCardTrigger;
}) {
  return <PlayerCardPopover trigger={children} label={name}>
    <PlayerCardDetails key={username} username={username} name={name} avatar={avatar} boardRating={boardRating} ratingCard={ratingCard}/>
  </PlayerCardPopover>;
}
