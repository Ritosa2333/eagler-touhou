import {useLocale} from '../../i18n';
import {Activity, ArrowUpRight, CircleHelp, Cloud, Gamepad2, Heart, Link2, UsersRound} from 'lucide-react';
import {siDiscord, siGithub, siQq} from 'simple-icons/icons';
import {SiteBrand} from './SiteBrand';
import './site-footer.css';

const labels = {
  'zh-CN': {play: '游玩', site: '本站', community: '社区', friends: '友链', single: '单机游戏', lobby: '联机大厅', about: '关于本站', status: '服务状态', donate: '捐赠'},
  'en': {play: 'Play', site: 'Site', community: 'Community', friends: 'Friends', single: 'Single player', lobby: 'Multiplayer', about: 'About', status: 'Service status', donate: 'Donate'},
  'ja': {play: 'プレイ', site: 'サイト', community: 'コミュニティ', friends: 'リンク', single: 'シングル', lobby: 'マルチプレイ', about: 'このサイトについて', status: '稼働状況', donate: '寄付'},
} as const;

export function SiteFooter({onDonation}: {onDonation?: () => void}) {
  const {locale} = useLocale();
  const text = labels[locale];
  return <footer className="site-footer"><div className="site-footer-grid">
    <a className="site-footer-brand" href="./" aria-label="Eagler Touhou"><SiteBrand/></a>
    <nav aria-label={text.play}><h2>{text.play}</h2><a href="./"><Gamepad2/>{text.single}</a><a href="./lobby.html"><UsersRound/>{text.lobby}</a></nav>
    <nav aria-label={text.site}><h2>{text.site}</h2><a href="./about.html"><CircleHelp/>{text.about}</a><a href="./uptime/"><Activity/>{text.status}</a>{onDonation && <button type="button" onClick={onDonation}><Heart/>{text.donate}</button>}</nav>
    <nav aria-label={text.community}><h2>{text.community}</h2><a href="https://discord.gg/HwVz9ZTSF"><BrandIcon path={siDiscord.path}/>Discord<ArrowUpRight className="site-footer-external"/></a><a href="https://github.com/YomotsuHisami/eagler-touhou"><BrandIcon path={siGithub.path}/>GitHub<ArrowUpRight className="site-footer-external"/></a><a href="https://qm.qq.com/q/eeUrxIltug"><BrandIcon path={siQq.path}/>QQ<ArrowUpRight className="site-footer-external"/></a></nav>
    <nav aria-label={text.friends}><h2>{text.friends}</h2><a href="https://dinebeta.bayuncloud.top/"><Link2/><span>DINE, a ph3sx reimpl <ArrowUpRight className="site-footer-external"/></span></a><a href="https://cloud.touhou.best/"><Cloud/>车万云<ArrowUpRight className="site-footer-external"/></a></nav>
  </div></footer>;
}

function BrandIcon({path}: {path: string}) {
  return <svg viewBox="0 0 24 24" aria-hidden="true" className="site-footer-social-icon"><path d={path}/></svg>;
}
