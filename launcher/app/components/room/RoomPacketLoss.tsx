import {useLocale} from '../../i18n';
import type {RoomOwnerSnapshot} from '../../services/multiplayer-room';
import type {RoomNetworkPresentation} from './types';

export function RoomPacketLoss({state, network}: {state: RoomOwnerSnapshot; network: RoomNetworkPresentation}) {
  const {t} = useLocale(), room = state.room;
  if (!room?.synced || room.phase !== 'lobby' || state.seat == null || state.spectatorRequested) return null;
  const peers = (room.seats || []).slice(0, room.playerCount).flatMap((seat, index) => seat && !seat.offline && index !== state.seat ? [{seat, index}] : []);
  const lanes = ['direct', 'turn'] as const;
  const running = peers.some(({seat}) => lanes.some(lane => network.loss(seat.clientId, lane).state === 'running'));
  const ready = state.connected && peers.some(({seat}) => lanes.some(lane => network.lossReady(seat.clientId, lane)));
  return <section className="room-packet-loss" aria-labelledby="mpPacketLossTitle">
    <header><div><h3 id="mpPacketLossTitle">{t('room.packetLoss')}</h3><p>{t('room.packetLossHint')}</p></div>
      <button id="mpPacketLossTest" type="button" disabled={!ready || running || state.launchBusy} onClick={() => network.testLoss()}>{t(running ? 'room.packetLossRunning' : 'room.packetLossTest')}</button>
    </header>
    <div role="status" aria-live="polite">{!peers.length ? <p>{t('room.waitPeer')}</p> : peers.map(({seat, index}) => <div className="room-packet-loss-peer" key={seat.clientId}>
      <strong>{state.nickname ? seat.name || `P${index + 1}` : `P${index + 1}`}</strong>
      {lanes.map(lane => {
        const result = network.loss(seat.clientId, lane);
        return <div className="room-packet-loss-row" key={lane} data-loss-lane={lane} data-loss-peer={seat.clientId}>
          <span>{t(`room.${lane}`)}</span>
          {result.state === 'complete' && result.outgoing && result.incoming ? <>
            <span title={`${result.outgoing.lost}/${result.outgoing.sent}`}>{t('room.packetLossOutgoing')} <b>{result.outgoing.percent.toFixed(1)}%</b></span>
            <span title={`${result.incoming.lost}/${result.incoming.sent}`}>{t('room.packetLossIncoming')} <b>{result.incoming.percent.toFixed(1)}%</b></span>
          </> : <span className="room-packet-loss-state">{t(`room.packetLoss.${result.state}`)}</span>}
        </div>;
      })}
    </div>)}</div>
  </section>;
}
