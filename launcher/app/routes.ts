import {index, route, type RouteConfig} from '@react-router/dev/routes';
/** Original launcher document addresses only. Assets and Runtime URLs are not
 * application fallbacks and must retain the existing publication routing. */
export default [
  index('routes/surface.tsx', {id: 'library'}),
  route('ui-review.html', 'routes/ui-review.tsx'),
  route('faq.html', 'routes/help.tsx', {id:'help-faq'}),
  route('about.html', 'routes/help.tsx', {id:'help-about'}),
  route('guide.html', 'routes/help.tsx', {id:'help-guide'}),
  route('multiplayer.html', 'routes/help.tsx', {id:'help-multiplayer'}),
  route('uptime', 'routes/uptime.tsx', {id: 'uptime'}),
  route('uptime/index.html', 'routes/uptime.tsx', {id: 'uptime-index'}),
  route('index.html', 'routes/surface.tsx', {id: 'library-html'}),
  route('en.html', 'routes/surface.tsx', {id: 'library-en'}),
  route('ja.html', 'routes/surface.tsx', {id: 'library-ja'}),
  route('login.html', 'routes/surface.tsx', {id: 'account'}),
  route('profile.html', 'routes/surface.tsx', {id: 'profile'}),
  route('lobby.html', 'routes/surface.tsx', {id: 'directory-html'}),
  route('lobby', 'routes/surface.tsx', {id: 'directory'}),
] satisfies RouteConfig;
