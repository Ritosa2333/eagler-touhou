import { parseMeasuredNetplayTiming } from '../contracts/netplay-timing.mjs';
import {
  isMultiplayerProductId,
  multiplayerConfigForProduct,
  multiplayerInputTimingPolicy,
} from '../contracts/product-catalog.mjs';
import { isUiLocale, translateUi, type UiMessageKey } from './i18n.mts';

let report: Record<string, unknown> | null = null;
let button: HTMLButtonElement | null = null;
let dialog: HTMLDialogElement | null = null;

export function resetCalibrationReport() {
  report = null;
  if (button) button.hidden = true;
  dialog?.close();
}

export function recordCalibrationReport(value: unknown, parent: HTMLElement, currentNetwork: () => string) {
  const timing = parseMeasuredNetplayTiming(value);
  if (!timing || timing.route === 'spectator') return;
  const raw = value as Record<string, unknown>;
  const calibration = raw.calibration as Record<string, unknown> | undefined;
  if (!calibration || !Array.isArray(calibration.players) || ![2, 3].includes(calibration.players.length)) return;
  const game = String(calibration.game);
  if (!isMultiplayerProductId(game)) return;
  const policy = multiplayerInputTimingPolicy(multiplayerConfigForProduct(game));
  if (!policy.measuredStartup || (!policy.rollback && timing.adonisMode !== 1)) return;
  const players = calibration.players as Array<Record<string, unknown>>;
  if (
    !players.every(
      (p, index) =>
        p.player === index &&
        Number.isInteger(p.p95Us) &&
        Number(p.p95Us) >= 1 &&
        Number(p.p95Us) <= 1_000_000 &&
        Number.isInteger(p.samples) &&
        Number(p.samples) >= 96 &&
        Number(p.samples) <= 120 &&
        p.lost === 120 - Number(p.samples),
    )
  )
    return;
  report = {
    schema: `eagler-touhou/${game.slice(0, 4)}-calibration-report/1`,
    game,
    ...timing,
    completedAt: calibration.completedAt,
    runtimeBuild: calibration.build,
    localPlayer: calibration.localPlayer,
    players: players.map((p) => ({
      player: p.player,
      p95Us: p.p95Us,
      samples: p.samples,
      lost: p.lost,
      minUs: p.minUs,
      maxUs: p.maxUs,
      meanUs: p.meanUs,
    })),
    formula:
      'B=max(1,ceil(floor(max(players.p95Us)/2)*60/1000000)); automatic hybrid P=min(configuredReserve,B-1), configuredReserve<=2; automatic D=B-P>=1',
    measurement: 'Startup input-channel RTT; arrival-triggered echo and high-resolution clock; frozen for this match',
    method: calibration.method,
    intervalMs: calibration.intervalMs,
    tailWaitMs: calibration.tailWaitMs,
    scope: calibration.scope,
    probes: calibration.probes,
    windowStart: calibration.windowStart,
    windowEnd: calibration.windowEnd,
    userAgent: navigator.userAgent,
  };
  if (!button) {
    button = document.createElement('button');
    button.id = 'netplayCalibrationReport';
    button.type = 'button';
    button.style.cssText =
      'pointer-events:auto;min-height:32px;padding:5px 9px;border:1px solid #777;border-radius:8px;background:#242421;color:#fff;font:inherit;cursor:pointer';
    parent.append(button);
    button.addEventListener('click', () => {
      if (!report) return;
      const language = document.documentElement.dataset.uiLocale;
      const t = (key: UiMessageKey) => translateUi(isUiLocale(language) ? language : 'zh-CN', key);
      dialog?.remove();
      dialog = document.createElement('dialog');
      dialog.id = 'netplayCalibrationDialog';
      dialog.style.cssText =
        'box-sizing:border-box;width:560px;max-width:calc(100vw - 24px);max-height:calc(100dvh - 24px);padding:18px;border:1px solid #777;border-radius:14px;background:#242421;color:#fff';
      const heading = document.createElement('h2');
      heading.textContent = t('calibration.reportTitle');
      const hint = document.createElement('p');
      hint.textContent = t('calibration.reportHint');
      const text = document.createElement('textarea');
      text.id = 'netplayCalibrationText';
      text.readOnly = true;
      text.setAttribute('aria-label', heading.textContent);
      text.style.cssText =
        'box-sizing:border-box;width:100%;height:40dvh;min-height:140px;background:#161614;color:#eee;padding:10px;font:12px/1.5 monospace';
      text.value = JSON.stringify(
        { ...report, copiedAt: new Date().toISOString(), liveNetworkDisplay: currentNetwork() },
        null,
        2,
      );
      const copy = document.createElement('button');
      copy.id = 'netplayCalibrationCopy';
      copy.type = 'button';
      copy.textContent = t('calibration.copy');
      copy.style.cssText = 'min-height:44px;margin:10px 12px 0 0;padding:8px 14px';
      copy.addEventListener('click', async () => {
        try {
          await navigator.clipboard.writeText(text.value);
          copy.textContent = t('calibration.copied');
        } catch {
          text.focus();
          text.select();
          copy.textContent = t('calibration.selected');
        }
      });
      const close = document.createElement('button');
      close.type = 'button';
      close.textContent = t('action.close');
      close.style.cssText = 'min-height:44px;padding:8px 14px';
      close.addEventListener('click', () => dialog?.close());
      dialog.append(heading, hint, text, copy, close);
      document.body.append(dialog);
      dialog.showModal();
    });
  }
  const language = document.documentElement.dataset.uiLocale;
  button.textContent = translateUi(isUiLocale(language) ? language : 'zh-CN', 'calibration.reportTrigger');
  button.hidden = false;
}
