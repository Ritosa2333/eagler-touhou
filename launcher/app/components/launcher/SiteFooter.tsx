import {useLocale} from '../../i18n';
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
    <nav aria-label={text.play}><h2>{text.play}</h2><a href="./">{text.single}</a><a href="./lobby.html">{text.lobby}</a></nav>
    <nav aria-label={text.site}><h2>{text.site}</h2><a href="./about.html">{text.about}</a><a href="./uptime/">{text.status}</a>{onDonation && <button type="button" onClick={onDonation}>{text.donate}</button>}</nav>
    <nav aria-label={text.community}><h2>{text.community}</h2><a href="https://discord.gg/HwVz9ZTSF">Discord ↗</a><a href="https://github.com/YomotsuHisami/eagler-touhou">GitHub ↗</a><a href="https://qm.qq.com/q/eeUrxIltug">QQ ↗</a></nav>
    <nav aria-label={text.friends}><h2>{text.friends}</h2><a href="https://dinebeta.bayuncloud.top/">DINE, a ph3sx reimpl ↗</a><a href="https://cloud.touhou.best/">车万云 ↗</a></nav>
  </div></footer>;
}
