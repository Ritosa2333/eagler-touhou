import type {Translate} from '../i18n';
import {ResourceDownloadError} from '../../src/launcher/resource-download.mts';
import type {RelayAttempt} from '../../src/launcher/relay-entry-routing.mts';

function reasonText(t: Translate, reason: string) {
  if (reason.startsWith('HTTP ')) return reason;
  return t(reason === 'timeout' ? 'network.timeout' : reason === 'connection' ? 'network.connectFailed' : 'network.badResponse');
}

export function downloadFailureDetail(error: unknown, t: Translate): string | undefined {
  // Package and launch errors may add context in separate layers.
  for (let depth = 0; error instanceof Error && depth < 8; depth++, error = error.cause) {
    if (error instanceof ResourceDownloadError) {
      return error.attempts.map(({stage, host, reason}) =>
        `${t(stage === 'routes' ? 'network.routeLookup' : 'network.transfer', {host})} (${reasonText(t, reason)})`).join(' | ');
    }
  }
}

export function relayFailureDetail(t: Translate, attempts?: readonly RelayAttempt[], code?: number): string {
  if (attempts?.length) return attempts.map(({host, reason}) => `${host} (${reasonText(t, reason)})`).join(' | ');
  return code && code !== 1006 ? t('network.relayClosed', {code}) : t('network.relayHandshake');
}
