import {useEffect, useRef, useState} from 'react';
import {Tabs} from 'radix-ui';
import {Button} from './button';
import {ChoiceGroup, ChoiceItem} from './choice-group';
import {RoomSeats} from '../components/room/RoomSeats';
import {RoomSpectators} from '../components/room/RoomSpectators';
import {RoomQuickChat} from '../components/room/RoomQuickChat';
import {RoomNetwork} from '../components/room/RoomNetwork';
import {RoomGameSettings} from '../components/room/RoomGameSettings';
import {RoomPanel} from '../components/room/RoomPanel';
import {RoomPersonal} from '../components/room/RoomPersonal';
import {multiplayerConfigForProduct} from '../../src/contracts/product-catalog.mts';
import {normalizeMultiplayerDisplayName} from '../../src/launcher/multiplayer-identity.mts';
import {createMultiplayerQuickChatModel, type MultiplayerQuickChatModel} from '../../src/launcher/multiplayer-quick-chat-model.mts';
import type {MultiplayerLobbySeat} from '../../src/launcher/multiplayer-lobby-snapshot.mts';
import type {RoomOwnerSnapshot} from '../services/multiplayer-room';
import type {RoomViewContext, RoomNetworkPresentation, RoomPanelKind} from '../components/room/types';
import '../components/room/room-workspace.css';

const context: RoomViewContext = {launched: false, previewReady: true, th09NetworkOverlayOpen: false, launchStage: null, touchEnabled: false, touchMovementMode: 'joystick'};
const seat = (clientId: string, name: string, loadout: number, ready: boolean): MultiplayerLobbySeat => ({clientId, name, loadout, ready, offline: false, controlMode: 'normal', mobileDevice: false, resource: {status: 'ready', stage: 'runtime', percent: 100}});
function initial(): RoomOwnerSnapshot {
  const seats = [seat('handbook-host', '演示房主', 0, false), seat('handbook-player', '演示玩家', 1, false), seat('handbook-ready', '已准备玩家', 0, true)];
  seats[1].resource = {status: 'preparing', stage: 'package', percent: 64};
  return {product: 'th10mp', room: {code: '1024', playerCount: 3, difficulty: 1, phase: 'lobby', created: true, synced: true, visibility: 'public', seats,
    spectators: [{clientId: 'handbook-viewer', name: '演示观众'}]}, epoch: 1, fromDirectory: false, connected: true, directorySupported: true, controlModesSupported: true,
    seat: 0, ready: false, spectatorRequested: false, preferredLoadout: 0, loadoutPending: false, displayName: '演示房主', roomSettingsOpen: false, startSerial: 0,
    preparation: {status: 'ready', stage: 'runtime', percent: 100, error: ''}, launchBusy: false, checkBusy: false, timingChoice: 'auto', rollbackEnabled: true,
    preferTurn: false, canStart: false, localClientId: 'handbook-host'};
}

/** Chinese maintainer examples use production views with document-local state. */
export function RoomExample({announce}: {announce(message: string): void}) {
  const [state, setState] = useState(initial), [tab, setTab] = useState('chat');
  const [chatSerial, setChatSerial] = useState(0);
  const [networkState, setNetworkState] = useState<'connected' | 'checking' | 'unavailable'>('connected');
  const [panel, setPanel] = useState<{kind: RoomPanelKind; peerId?: string} | null>(null), [chat, setChat] = useState<MultiplayerQuickChatModel | null>(null);
  const trigger = useRef<HTMLElement | null>(null);
  useEffect(() => {
    const model = createMultiplayerQuickChatModel({send: message => {
      const current = model.getSnapshot().context;
      model.receive({...message, room: current?.room, seat: current?.localSeat ?? -1, spectator: current?.localSpectator, clientId: 'handbook-host'});
    }, voice() {},
      timers: {setTimeout: (callback, delay) => window.setTimeout(callback, delay), clearTimeout: timer => window.clearTimeout(timer)}});
    setChat(model); return () => model.dispose();
  }, []);
  useEffect(() => {chat?.update({visible: true, room: '1024', serial: chatSerial, localSeat: state.seat, seats: state.room?.seats ?? [],
    spectators: state.room?.spectators, localClientId: state.localClientId, localSpectator: state.spectatorRequested, connected: true, language: 'zh-CN', inRoom: true});}, [chat, state, chatSerial]);
  function updateRoom(patch: Partial<NonNullable<RoomOwnerSnapshot['room']>>) {setState(previous => ({...previous, room: {...previous.room!, ...patch}}));}
  function removePlayer(index: number, spectator = false) {
    const player = state.room?.seats?.[index]; if (!player) return;
    updateRoom({seats: state.room!.seats!.map((value, i) => i === index ? null : value),
      spectators: spectator ? [...state.room!.spectators ?? [], player] : state.room!.spectators});
    announce(spectator ? '演示玩家已移到观战席' : '演示玩家已移除');
  }
  const actions = {
    getSnapshot: () => state,
    async takeSeat(index: number) {
      updateRoom({seats: state.room!.seats!.map((value, i) => i === index ? seat(state.localClientId, state.displayName, 0, false) : value?.clientId === state.localClientId ? null : value)});
      setState(previous => ({...previous, seat: index, spectatorRequested: false}));
    },
    movePlayerToSpectator: (index: number) => removePlayer(index, true),
    async removePlayer(index: number) {removePlayer(index);},
    async removeSpectator(clientId: string) {updateRoom({spectators: state.room!.spectators!.filter(value => value.clientId !== clientId)});},
    async spectate() {
      updateRoom({seats: state.room!.seats!.map(value => value?.clientId === state.localClientId ? null : value), spectators: [...state.room!.spectators ?? [], {clientId: state.localClientId, name: state.displayName}]});
      setState(previous => ({...previous, seat: null, spectatorRequested: true}));
    },
    async leaveSpectator() {updateRoom({spectators: state.room!.spectators!.filter(value => value.clientId !== state.localClientId)}); setState(previous => ({...previous, spectatorRequested: false}));},
    setRoomSettings: updateRoom,
    timingRecommendation: () => ({inputDelay: 0, targetRollbackFrames: 12, networkFrames: 0, mobileSeats: 0}),
    setRollbackEnabled: (value: boolean) => setState(previous => ({...previous, rollbackEnabled: value})),
    setTimingChoice: (value: 'auto' | number) => setState(previous => ({...previous, timingChoice: value})),
    setDisplayName(value: string) {
      const displayName = normalizeMultiplayerDisplayName(value);
      setState(previous => ({...previous, displayName, room: {...previous.room!, seats: previous.room!.seats!.map(member => member?.clientId === previous.localClientId ? {...member, name: displayName} : member)}}));
      return displayName;
    },
    changeLoadout(delta: number) {
      const count = multiplayerConfigForProduct('th10mp')!.loadouts.length;
      setState(previous => {const preferredLoadout = (previous.preferredLoadout + delta + count) % count;
        return {...previous, preferredLoadout, room: {...previous.room!, seats: previous.room!.seats!.map(member => member?.clientId === previous.localClientId ? {...member, loadout: preferredLoadout} : member)}};
      });
    },
    async standUp() {updateRoom({seats: state.room!.seats!.map(member => member?.clientId === state.localClientId ? null : member)});setState(previous => ({...previous, seat: null, ready: false}));},
    async checkGame() {announce('此处不启动 Native。实际检查从联机房间进入。');},
    setPreferTurn: (preferTurn: boolean) => setState(previous => ({...previous, preferTurn})),
  };
  const network: Pick<RoomNetworkPresentation, 'capabilities' | 'metric' | 'retry' | 'loss' | 'lossReady' | 'testLoss'> = {
    capabilities: () => ({supported: true, rtcAvailable: true, turnConfigured: true}),
    metric: (_peer, lane) => ({state: networkState, rtt: networkState === 'connected' ? lane === 'direct' ? 32 : 86 : null, jitter: networkState === 'connected' ? 3 : null, at: Date.now()}),
    retry: () => setNetworkState('checking'),
    loss: () => ({state: 'idle', sent: 0}), lossReady: () => false, testLoss() {},
  };
  const player = state.room?.seats?.[1];
  const appearance = !player ? 'empty' : player.offline ? 'reconnecting' : player.resource?.status !== 'ready' ? 'loading' : player.ready ? 'ready' : 'unready';
  return <>
    <p className="catalog-muted">生产房间部件 | 操作只改变本页示例。重新载入或重置后恢复。</p>
    <div className="catalog-row"><span>P2 状态</span><ChoiceGroup type="single" className="catalog-choice-group" aria-label="P2 示例状态" value={appearance} onValueChange={value => {
      if (!value) return;
      const next = seat('handbook-player', '演示玩家', 1, value === 'ready'); next.offline = value === 'reconnecting';
      if (value === 'loading') next.resource = {status: 'preparing', stage: 'package', percent: 64};
      updateRoom({seats: state.room!.seats!.map((valueAtSeat, i) => i === 1 ? value === 'empty' ? null : next : valueAtSeat)});
    }}>{[['loading', '加载'], ['reconnecting', '重连'], ['unready', '未准备'], ['ready', '已准备'], ['empty', '空席']].map(([value, label]) => <ChoiceItem value={value} key={value}>{label}</ChoiceItem>)}</ChoiceGroup>
      <Button variant="outline" onClick={() => {setState(initial());setNetworkState('connected');setTab('chat');setPanel(null);setChatSerial(serial => serial + 1);announce('');}}>重置房间示例</Button></div>
    <div id="player" className="room-workspace catalog-room-workspace" data-launcher-document=""><div id="mpRoomView" className="room-workspace-view">
      <div className="room-workspace-roster"><RoomSeats state={state} service={actions} network={network} context={context} openPanel={(kind, element, peerId) => {trigger.current = element;setTab('chat');setPanel({kind, peerId});}}/></div>
      <Tabs.Root value={tab} onValueChange={setTab}><Tabs.List className="ui-tabs-list" aria-label="房间区域示例">
        {[['chat', '快捷发言'], ['spectators', '观战席'], ['game', '房间设置'], ['network', '网络详情']].map(([value, label]) => <Tabs.Trigger className="ui-tabs-trigger" key={value} value={value}>{label}</Tabs.Trigger>)}
      </Tabs.List><Tabs.Content value="chat">{chat && <RoomQuickChat model={chat}/>}</Tabs.Content>
        <Tabs.Content value="spectators"><RoomSpectators state={state} service={actions} hidden={false}/></Tabs.Content>
        <Tabs.Content value="game"><RoomGameSettings state={state} service={actions} hidden={false}/></Tabs.Content>
        <Tabs.Content value="network"><ChoiceGroup type="single" className="catalog-choice-group" aria-label="网络示例状态" value={networkState} onValueChange={value => {if (value === 'connected' || value === 'checking' || value === 'unavailable') setNetworkState(value);}}>{[['connected', '已测量'], ['checking', '检查中'], ['unavailable', '不可用']].map(([value, label]) => <ChoiceItem value={value} key={value}>{label}</ChoiceItem>)}</ChoiceGroup><RoomNetwork state={state} network={network} context={context} hidden={false}/></Tabs.Content>
      </Tabs.Root>
      <RoomPanel open={!!panel} kind={panel?.kind ?? 'personal'} peerId={panel?.peerId} onCloseRequest={() => setPanel(null)} onClosed={() => {}} trigger={trigger}>
        {panel?.kind === 'personal' && <RoomPersonal state={state} service={actions} network={network} assetUrl={path => path} th09NetworkOverlayOpen={false} hidden={false}/>}
        {panel?.kind === 'network' && <RoomNetwork state={state} network={network} context={context} peerId={panel.peerId} hidden={false}/>}
        {panel?.kind === 'spectators' && <RoomSpectators state={state} service={actions} hidden={false}/>}
        {panel?.kind === 'game' && <RoomGameSettings state={state} service={actions} hidden={false}/>}
      </RoomPanel>
    </div></div>
  </>;
}
