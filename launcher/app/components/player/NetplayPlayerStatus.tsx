import type {GameplayNetworkSnapshot} from '../../models/gameplay-network';
import {useLocale} from '../../i18n';
import './netplay-player-status.css';

/** Present variation as a small subscript following the primary RTT. */
export function NetplayPlayerStatus({visible, rows}: {visible: boolean; rows: GameplayNetworkSnapshot['playerStatus']}) {
  const {t} = useLocale();
  const latency = t('diagnostics.roundTripLatency'), variation = t('diagnostics.latencyVariation');
  return <aside className="netplay-player-status" id="netplayPlayerStatus" aria-label={t('multiplayer.connection')} hidden={!visible}>
    {rows.map(row => <span className="netplay-peer-status" key={row.label} title={row.title}>
      <span className="netplay-peer-label">{row.label}</span>
      {row.reconnecting ? <span className="netplay-peer-route">{t('diagnostics.peerReconnecting')}</span> : <>
        <span className="netplay-peer-route">{row.route === 'connecting' ? t('diagnostics.transportConnecting') : row.route}</span>
        {(row.route !== 'connecting' || row.rttMs !== null) && <span className="netplay-peer-values">
          <span className="netplay-peer-rtt" title={`${latency} ${row.rttMs ?? '--'}ms`}>
            <span className="visually-hidden">{latency} </span><strong>{row.rttMs ?? '--'}</strong><small>ms</small>
          </span>
          {row.rttMs !== null && <span className="netplay-peer-variation" title={`${variation} ${row.variationMs ?? '--'}ms`}>
            <span className="visually-hidden">{variation} </span>
            <span>{row.variationMs ?? '--'}<small>ms</small></span>
          </span>}
        </span>}
      </>}
    </span>)}
  </aside>;
}
