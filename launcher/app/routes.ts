import {index, route, type RouteConfig} from '@react-router/dev/routes';
import {UI_REVIEW_DOCUMENT, uiReviewEnabled} from '../lib/frontend-static-manifest.mjs';
import {MULTIPLAYER_GUIDE_PATH} from '../lib/multiplayer-guide.mjs';
/** Original launcher document addresses only. Assets and Runtime URLs are not
 * application fallbacks and must retain the existing publication routing. */
export default [
  index('routes/surface.tsx', {id: 'library'}),
  ...(uiReviewEnabled() ? [route(UI_REVIEW_DOCUMENT.path.slice(1), 'routes/ui-review.tsx')] : []),
  route('privacy.html', 'routes/help.tsx', {id:'help-privacy'}),
  route('faq.html', 'routes/help.tsx', {id:'help-faq'}),
  route('about.html', 'routes/help.tsx', {id:'help-about'}),
  route('guide.html', 'routes/help.tsx', {id:'help-guide'}),
  route('multiplayer.html', 'routes/help.tsx', {id:'help-multiplayer'}),
  route(MULTIPLAYER_GUIDE_PATH.slice(1), 'routes/help.tsx', {id:'multiplayer-guide'}),
  route(`${MULTIPLAYER_GUIDE_PATH.slice(1)}/:topic`, 'routes/help.tsx', {id:'multiplayer-guide-topic'}),
  route('uptime', 'routes/uptime.tsx', {id: 'uptime'}),
  route('uptime/index.html', 'routes/uptime.tsx', {id: 'uptime-index'}),
  route('index.html', 'routes/surface.tsx', {id: 'library-html'}),
  route('en.html', 'routes/surface.tsx', {id: 'library-en'}),
  route('ja.html', 'routes/surface.tsx', {id: 'library-ja'}),
  route('login.html', 'routes/surface.tsx', {id: 'account'}),
  route('u/:username', 'routes/surface.tsx', {id:'public-profile'}),
  route('profile.html', 'routes/surface.tsx', {id: 'profile'}),
  route('lobby.html', 'routes/surface.tsx', {id: 'directory-html'}),
  route('lobby', 'routes/surface.tsx', {id: 'directory'}),
] satisfies RouteConfig;
