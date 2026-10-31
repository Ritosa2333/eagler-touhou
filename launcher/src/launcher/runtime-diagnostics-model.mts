import { translateUi, type UiMessageKey } from './i18n.mts';
export interface BrowserEnvironmentInput {
  userAgent?: string | null;
  platform?: string | null;
  userAgentDataPlatform?: string | null;
  mobile?: boolean | null;
  brave?: boolean | null;
}

export function runtimeDiagnosticsVisibleByDefault(
  testBuild: unknown,
  launched: unknown,
  preference: unknown = null,
): boolean {
  const enabled = preference === true || (preference !== false && testBuild === true);
  return enabled && launched === true;
}

export function compactDiagnosticText(value: unknown, maxLength = 96): string {
  const text = String(value ?? '')
    .replace(/\s+/g, ' ')
    .trim();
  const limit = Math.max(1, Math.trunc(maxLength) || 96);
  const characters = [...text];
  return characters.length <= limit ? text : `${characters.slice(0, Math.max(1, limit - 1)).join('')}…`;
}

export function describeBrowserEnvironment(
  input: BrowserEnvironmentInput,
  t: (key: string) => string = (key) => translateUi('zh-CN', key as UiMessageKey),
) {
  const ua = String(input.userAgent || '');
  const match = (regex: RegExp): string => regex.exec(ua)?.[1] || '';
  const chromeVersion = match(/(?:Chrome|CriOS)\/([\d.]+)/i);
  const isWebView = /;\s*wv\)/i.test(ua) || (/\bVersion\/4\.0\b/i.test(ua) && /\bChrome\//i.test(ua));
  const browserCandidates: ReadonlyArray<readonly [RegExp, string]> = [
    [/\bVia(?:Browser)?[/]?([\d.]*)/i, 'Via'],
    [/\bQuark\/([\d.]+)/i, 'Quark'],
    [/\bHuaweiBrowser\/([\d.]+)/i, 'Huawei Browser'],
    [/\bMiuiBrowser\/([\d.]+)/i, 'Mi Browser'],
    [/\bVivoBrowser\/([\d.]+)/i, 'vivo Browser'],
    [/\bHeyTapBrowser\/([\d.]+)/i, 'HeyTap Browser'],
    [/\bSamsungBrowser\/([\d.]+)/i, 'Samsung Internet'],
    [/\bEdgA\/([\d.]+)/i, 'Edge'],
    [/\bEdgiOS\/([\d.]+)/i, 'Edge'],
    [/\bEdg\/([\d.]+)/i, 'Edge'],
    [/\bOPR\/([\d.]+)/i, 'Opera'],
    [/\bVivaldi\/([\d.]+)/i, 'Vivaldi'],
    [/\bUCBrowser\/([\d.]+)/i, 'UC Browser'],
    [/\bMQQBrowser\/([\d.]+)/i, 'QQ Browser'],
    [/\bDuckDuckGo\/([\d.]+)/i, 'DuckDuckGo'],
    [/\bFirefox\/([\d.]+)/i, 'Firefox'],
    [/\bFxiOS\/([\d.]+)/i, 'Firefox'],
    [/\bCriOS\/([\d.]+)/i, 'Chrome'],
    [/\bChrome\/([\d.]+)/i, isWebView ? 'Android WebView' : 'Chrome'],
    [/\bVersion\/([\d.]+).*\bSafari\//i, 'Safari'],
  ];
  let browser = t('diagnostics.unknownBrowser');
  for (const [regex, name] of browserCandidates) {
    const version = regex.exec(ua)?.[1];
    if (version != null) {
      browser = `${name}${version ? ` ${version}` : ''}`;
      break;
    }
  }
  if (browser.startsWith('Chrome') && input.brave === true)
    browser = `Brave${chromeVersion ? ` ${chromeVersion}` : ''}`;
  const androidVersion = match(/\bAndroid\s+([^;)\s]+)/i);
  const iosVersion = match(/\b(?:CPU(?: iPhone)? OS|iPhone OS)\s+([\d_]+)/i).replaceAll('_', '.');
  const platform = androidVersion
    ? `Android ${androidVersion}`
    : iosVersion
      ? `iOS ${iosVersion}`
      : String(input.userAgentDataPlatform || input.platform || '').trim();
  const firefoxVersion = match(/\bFirefox\/([\d.]+)/i);
  const safariVersion = match(/\bVersion\/([\d.]+).*\bSafari\//i);
  const engine = chromeVersion
    ? `Chromium ${chromeVersion}`
    : firefoxVersion
      ? `Gecko ${firefoxVersion}`
      : safariVersion
        ? `WebKit ${safariVersion}`
        : '';
  const mobile = input.mobile === true || /\bMobile\b/i.test(ua);
  const tbsVersion = match(/\bTBS\/([\d.]+)/i);
  const traits = [
    engine,
    platform,
    tbsVersion ? `TBS ${tbsVersion}` : '',
    isWebView ? 'WebView' : mobile ? t('diagnostics.mobileDevice') : t('diagnostics.desktopDevice'),
  ].filter(Boolean);
  return { browser, ua: traits.join(' / ') || '--', raw: ua };
}

export function compactRendererLabel(raw: unknown): string {
  const value = String(raw || '')
    .replace(/\s+/g, ' ')
    .trim();
  if (!value) return '--';
  const angle = /^ANGLE \((.*)\)$/.exec(value);
  if (!angle) return value.slice(0, 72);
  const parts = angle[1]
    .split(',')
    .map((part) => part.trim())
    .filter(Boolean);
  const gpu = parts.find((part) => /Adreno|Mali|GeForce|Radeon|Intel|Apple|PowerVR|SwiftShader|llvmpipe/i.test(part));
  return String(gpu || parts[1] || parts[0] || value).slice(0, 72);
}

type StatsRecord = Record<string, unknown>;
export interface StatsCollection {
  values(): Iterable<unknown>;
  get(id: string): unknown;
}

export function selectedRtcPair(stats: StatsCollection): StatsRecord | null {
  for (const raw of stats.values()) {
    const report = raw as StatsRecord;
    if (report.type === 'transport' && typeof report.selectedCandidatePairId === 'string') {
      const pair = stats.get(report.selectedCandidatePairId);
      if (pair && typeof pair === 'object') return pair as StatsRecord;
    }
  }
  for (const raw of stats.values()) {
    const report = raw as StatsRecord;
    if (report.type === 'candidate-pair' && report.nominated === true && report.state === 'succeeded') return report;
  }
  return null;
}

export function appendRttSample(previous: readonly number[], rttMs: number, limit = 20) {
  if (!Number.isFinite(rttMs) || rttMs < 0) return { samples: [...previous], rttMs: null, variationMs: null };
  const capacity = Math.max(1, Math.trunc(limit) || 20);
  const samples = [...previous, rttMs].slice(-capacity);
  const sorted = [...samples].sort((a, b) => a - b);
  const p50 = sorted[Math.floor((sorted.length - 1) * 0.5)];
  const p95 = sorted[Math.floor((sorted.length - 1) * 0.95)];
  return { samples, rttMs, variationMs: samples.length >= 3 ? Math.max(0, p95 - p50) : null };
}

export interface CompactNetplayPeerStatusInput {
  player: unknown;
  route?: unknown;
  rttMs?: unknown;
  variationMs?: unknown;
  connected?: unknown;
}

export interface NetplayPeerStatus {
  label: string;
  route: string;
  reconnecting: boolean;
  rttMs: number | null;
  variationMs: number | null;
  text: string;
}

export function describeNetplayPeerStatus(input: CompactNetplayPeerStatusInput): NetplayPeerStatus {
  const player = Number(input.player);
  const label = Number.isFinite(player) && player >= 0 ? `P${Math.trunc(player) + 1}` : 'P?';
  const rawRoute = String(input.route || 'connecting')
    .trim()
    .toLowerCase();
  const route = ['direct', 'turn', 'relay', 'rtc'].includes(rawRoute) ? rawRoute : 'connecting';
  const measured = (value: unknown): number | null => {
    if (value == null || value === '') return null;
    const number = Number(value);
    return Number.isFinite(number) && number >= 0 ? Math.round(number) : null;
  };
  const reconnecting = input.connected === false;
  const rttMs = reconnecting ? null : measured(input.rttMs);
  const variationMs = rttMs === null ? null : measured(input.variationMs);
  const text = reconnecting
    ? `${label} reconnecting`
    : rttMs !== null
      ? `${label} ${route} ${rttMs}${variationMs === null ? '' : `/${variationMs}`}ms`
      : route === 'connecting'
        ? `${label} connecting`
        : `${label} ${route} --ms`;
  return { label, route, reconnecting, rttMs, variationMs, text };
}

export function compactNetplayPeerStatus(input: CompactNetplayPeerStatusInput): string {
  return describeNetplayPeerStatus(input).text;
}

export interface NetplayConnectionPeer {
  pc?: {
    connectionState?: unknown;
    iceConnectionState?: unknown;
  } | null;
  inputOpen?: boolean;
  controlOpen?: boolean;
}

export interface NetplayConnectionPeerState {
  disconnected?: boolean;
  isRecovering?(): boolean;
  relay?: { readyState?: unknown } | null;
  peers?: { get?(player: number): NetplayConnectionPeer | undefined } | null;
}

export interface NetplayConnectionInput {
  english?: boolean;
  translate?(key: string): string;
  spectator?: boolean;
  failed?: boolean;
  nativeFailed?: boolean;
  error?: unknown;
  transport?: unknown;
  path?: unknown;
  peerState: NetplayConnectionPeerState;
  playerCount?: unknown;
  localPlayer?: unknown;
  connectedOnce?: boolean;
  webSocketOpenState?: number;
}

export interface NetplayConnectionPeerRow {
  player: number;
  status: string;
  detail: string;
  disconnected: boolean;
}

export interface NetplayConnectionView {
  ended?: boolean;
  hidden: boolean;
  title: string;
  summary: string;
  peerRows: NetplayConnectionPeerRow[];
  warning: string;
  reconnecting: boolean;
  connectedOnce: boolean;
  showRouteWarning: boolean;
}

export function describeNetplayConnection(input: NetplayConnectionInput): NetplayConnectionView {
  const peerState = input.peerState || {};
  const transport = String(input.transport || 'connecting');
  const openState = Number.isFinite(input.webSocketOpenState) ? Number(input.webSocketOpenState) : 1;
  const connectedOnce = input.connectedOnce === true;
  const t =
    input.translate ?? ((key: string) => translateUi(input.english === true ? 'en' : 'zh-CN', key as UiMessageKey));

  if (input.spectator === true) {
    const relayReady = Number(peerState.relay?.readyState) === openState;
    const failed = input.failed === true;
    return {
      ended: input.nativeFailed === true,
      hidden: relayReady && !input.nativeFailed,
      title: input.nativeFailed
        ? t('connection.playbackStopped')
        : relayReady
          ? ''
          : failed
            ? t('connection.spectatorLost')
            : t('connection.spectatorConnecting'),
      summary: input.nativeFailed
        ? t('connection.playbackStoppedHint')
        : relayReady
          ? ''
          : failed
            ? String(input.error || t('connection.spectatorFailed'))
            : t('connection.spectatorWaiting'),
      peerRows: [],
      warning: '',
      reconnecting: !input.nativeFailed && !relayReady && failed,
      connectedOnce,
      showRouteWarning: false,
    };
  }

  const playerCount = Number(input.playerCount);
  const expected = Math.max(1, (Number.isFinite(playerCount) ? playerCount : 2) - 1);
  const localValue = Number(input.localPlayer);
  const localPlayer = Math.max(0, Number.isFinite(localValue) ? localValue : 0);
  const peerRows: NetplayConnectionPeerRow[] = [];
  let rtcReadyPeers = 0;
  for (let player = 0; player < expected + 1; player++) {
    if (player === localPlayer) continue;
    const peer = peerState.peers?.get?.(player);
    const pcState = String(peer?.pc?.connectionState || peer?.pc?.iceConnectionState || '');
    const channelsReady = peer?.inputOpen === true && peer?.controlOpen === true;
    if (channelsReady) rtcReadyPeers++;
    const disconnected =
      ['disconnected', 'failed', 'closed'].includes(pcState) ||
      (connectedOnce && transport === 'rtc' && !channelsReady);
    const status = disconnected
      ? t('connection.peerReconnecting')
      : channelsReady
        ? t('connection.peerConnected')
        : pcState === 'checking' || pcState === 'connecting'
          ? t('connection.peerConnecting')
          : t('connection.peerWaiting');
    peerRows.push({
      player,
      status,
      disconnected,
      detail: '',
    });
  }

  const relayReady = transport === 'relay' && Number(peerState.relay?.readyState) === openState;
  const recovering = peerState.isRecovering?.() === true;
  const ended = peerState.disconnected === true || input.nativeFailed === true;
  const allReady = !ended && !recovering && (relayReady || (transport === 'rtc' && rtcReadyPeers === expected));
  if (allReady) {
    return {
      hidden: true,
      title: '',
      summary: '',
      peerRows,
      warning: '',
      reconnecting: false,
      connectedOnce: true,
      showRouteWarning: false,
    };
  }

  const disconnectedRows = peerRows.filter((row) => row.disconnected);
  const reconnecting =
    !ended && (recovering || (connectedOnce && (disconnectedRows.length > 0 || input.failed === true)));
  return {
    ended,
    hidden: false,
    title: input.nativeFailed
      ? t('connection.stopped')
      : ended
        ? t('connection.lost')
        : reconnecting
          ? t('connection.reconnecting')
          : t('connection.connecting'),
    summary: input.nativeFailed
      ? t('connection.stoppedHint')
      : ended
        ? t('connection.lostHint')
        : reconnecting
          ? t('connection.reconnectingHint')
          : t('connection.waitingHint'),
    peerRows:
      peerState.disconnected === true
        ? peerRows.map((row) => ({ ...row, status: t('connection.peerDisconnected'), disconnected: true }))
        : peerRows,
    warning: '',
    reconnecting,
    connectedOnce,
    showRouteWarning: false,
  };
}
