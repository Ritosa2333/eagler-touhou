import {Button as UiButton} from '../../ui/controls';
import {defaultTh15ChapterThreshold} from "../../../src/launcher/th15-chapter-reward.mts";
import {TH05_INITIAL_STOCK} from '../../../src/launcher/th05-initial-stock.mts';
import type {RoomOwnerSnapshot, MultiplayerRoomService} from '../../services/multiplayer-room';
import {useLocale} from '../../i18n';
import {RoomChoiceGroup} from './RoomChoiceGroup';
import {roomPresentation} from './presentation';
import {MainSelect} from '../launcher/MainSelect';
import {RoomTiming, type RoomTimingActions} from './RoomTiming';
import {OptionSwitch} from '../settings/TouchSettingsFields';
import {Th19VersusSettings} from './Th19VersusSettings';
import {SettingsSelect} from '../settings/SettingsSelect';
export function RoomGameSettings({state, service, hidden}: {state: RoomOwnerSnapshot; service: Pick<MultiplayerRoomService, 'setRoomSettings' | 'takeSeat'> & RoomTimingActions; hidden: boolean}) {
  const {t} = useLocale(), {room, ready, owner, config} = roomPresentation(state, t)!;
  const disabled = !ready || !owner || room.phase !== 'lobby';
  return <section className="mp-room-settings room-grouped-settings options-settings" id="mpRoomSettings" hidden={hidden} data-room-panel="game">
    <section className="room-settings-group"><h3>{t('room.basicSettings')}</h3>
      <div className="room-basic-fields">
        <label>{t('room.visibility')}<MainSelect id="mpRoomVisibility" aria-label={t('room.visibility')} className="option-select" value={room.visibility || 'public'} disabled={disabled} onChange={event => service.setRoomSettings({visibility: event.target.value as 'public' | 'private'})}>
          <option value="public">{t('room.public')}</option><option value="private">{t('room.private')}</option>
        </MainSelect></label>
        <label>{t('multiplayer.players')}<MainSelect id="mpRoomPlayerCount" aria-label={t('multiplayer.players')} className="option-select" value={room.playerCount} disabled={disabled || config.playerCounts.length < 2} onChange={event => service.setRoomSettings({playerCount: Number(event.target.value) as 2 | 3})}>
          {config.playerCounts.map(count => <option key={count} value={count}>{count}P</option>)}
        </MainSelect></label>
      </div>
      <p className="room-setting-description" hidden={room.visibility !== 'private'}>{t('room.privateHint')}</p>
      {config.inGameSelection ? <p className="room-setting-description">{t('room.inGameSelectionHint')}</p> : <RoomChoiceGroup title={<><span>{t('multiplayer.difficulty')}</span>{room.challengeMode && <span className="room-challenge-on">{t('room.challengeEnabled')}</span>}</>} name="difficulty" valueAttribute="data-mp-difficulty" choices={config.difficulties.map((label, index) => ({value:index,label,selected:room.difficulty === index,disabled}))} onSelect={difficulty => service.setRoomSettings({difficulty})}/>}
    </section>
    <section className="room-settings-group"><h3>{t('room.gameSettingsGroup')}</h3>
      <RoomTiming state={state} service={service}/><RoomTiming state={state} service={service} settings/>
      {state.product === 'th19mp' && <Th19VersusSettings rules={room.th19Rules} disabled={disabled} onChange={th19Rules=>service.setRoomSettings({th19Rules})}/>}
      {state.product === 'th05mp' && <>
        <fieldset className="room-basic-fields" aria-describedby="mpTh05InitialStockHint">
          <SettingsSelect id="mpTh05InitialSpares" label={t('room.th05InitialSpares')} value={room.initialSpares ?? TH05_INITIAL_STOCK.spares}
            disabled={disabled} onChange={event => service.setRoomSettings({initialSpares: Number(event.target.value)})}>
            {[0, 1, 2, 3, 4, 5].map(value => <option key={value} value={value}>{value}</option>)}
          </SettingsSelect>
          <SettingsSelect id="mpTh05InitialBombs" label={t('room.th05InitialBombs')} value={room.initialBombs ?? TH05_INITIAL_STOCK.bombs}
            disabled={disabled} onChange={event => service.setRoomSettings({initialBombs: Number(event.target.value)})}>
            {[0, 1, 2, 3].map(value => <option key={value} value={value}>{value}</option>)}
          </SettingsSelect>
        </fieldset>
        <p className="room-setting-description" id="mpTh05InitialStockHint">{t('room.th05InitialStockHint')}</p>
      </>}
      {state.product === 'th15mp' && <><label className="room-reward-field" htmlFor="mpTh15ChapterThreshold">{t('room.th15ChapterThreshold')}<MainSelect className="option-select" id="mpTh15ChapterThreshold" aria-label={t('room.th15ChapterThreshold')} aria-describedby="mpTh15ChapterGraze" value={String(room.chapterThreshold ?? defaultTh15ChapterThreshold(room.playerCount))} disabled={disabled} onChange={event => service.setRoomSettings({chapterThreshold:Number(event.target.value)})}>
        {Array.from({length:19},(_,index)=>500000+index*250000).map(value=><option key={value} value={value}>{value.toLocaleString()}</option>)}
      </MainSelect></label><p className="room-setting-description" id="mpTh15ChapterGraze">{t('room.th15ChapterGraze',{count:(room.chapterThreshold ?? defaultTh15ChapterThreshold(room.playerCount))/5000})}</p></>}
    </section>
    <section className="room-settings-group"><h3>{t('room.ruleSettings')}</h3>
      {(['cheat','challenge'] as const).map(kind => <div className="room-rule-field" key={kind} hidden={kind === 'challenge' && config.gameplay !== 'cooperative'}><div><strong>{t(kind === 'cheat' ? 'lobby.noCheat' : 'room.challengeMode')}</strong><p className="room-setting-description" id={kind === 'cheat' ? 'mpRoomCheatHint' : 'mpRoomChallengeHint'}>{t(kind === 'cheat' ? 'room.cheatHint' : 'room.challengeHint')}</p></div>
        <OptionSwitch id={kind === 'cheat' ? 'mpRoomCheatToggle' : 'mpRoomChallengeToggle'} label={t(kind === 'cheat' ? 'lobby.noCheat' : 'room.challengeMode')} checked={kind === 'cheat' ? !!room.disableCheatMovement : !!room.challengeMode} disabled={disabled} onChange={() => service.setRoomSettings(kind === 'cheat' ? {disableCheatMovement:!room.disableCheatMovement} : {challengeMode:!room.challengeMode})}/>
      </div>)}
      {state.product === 'th09mp' && <div className="room-rule-field"><div><strong>{t('room.disableLocalChargePriority')}</strong><p className="room-setting-description" id="mpLocalChargeReplayHint">{t('room.localChargeReplayHint')}</p></div>
        <OptionSwitch id="mpDisableLocalChargePriority" label={t('room.disableLocalChargePriority')} describedBy="mpLocalChargeReplayHint" checked={!!room.disableLocalChargePriority} disabled={disabled} onChange={()=>service.setRoomSettings({disableLocalChargePriority:!room.disableLocalChargePriority})}/>
      </div>}
    </section>
    {!owner && <p className="room-setting-description" id="mpRoomSettingsHint">{t(!room.seats?.[0] ? 'room.hostAvailable' : 'multiplayer.ownerRemoteHint')}</p>}
    <UiButton id="mpTakeHostSeat" type="button" hidden={!!room.seats?.[0] || owner} disabled={!ready || room.phase !== 'lobby'} onClick={()=>{void service.takeSeat(0);}}>{t('room.takeHostSeat')}</UiButton>
  </section>;
}
