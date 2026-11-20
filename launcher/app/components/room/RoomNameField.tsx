import {useState} from 'react';
import {ROOM_NAME_MAX_LENGTH, normalizeRoomName} from '../../../../common/contracts/room-name.mjs';
import {useLocale} from '../../i18n';
import type {RoomOwnerSnapshot, MultiplayerRoomService} from '../../services/multiplayer-room';
import {Button} from '../../ui/controls';
import {Input} from '../../ui/input';

export function RoomNameField({id, text, onText}: {
  id: string; text: string; onText(value: string): void;
}) {
  const {t} = useLocale();
  const invalid = !!text.trim() && !normalizeRoomName(text);
  return <div className="lobby-field grid min-w-0 gap-1.5">
    <label htmlFor={id}>{t('roomName.label')}</label>
    <Input id={id} type="text" value={text} maxLength={128} autoComplete="off"
      placeholder={t('directory.regularRules')} aria-invalid={invalid}
      aria-describedby={invalid ? id+'Limit' : undefined} onChange={e=>onText(e.target.value)}/>
    {invalid && <small id={id+'Limit'}>{t('roomName.limit',{count:ROOM_NAME_MAX_LENGTH})}</small>}
  </div>;
}

export function RoomNameSettings({name, status, save}: {
  name: string | null; status: RoomOwnerSnapshot['roomNameStatus'];
  save: MultiplayerRoomService['setRoomName'];
}) {
  const {t} = useLocale();
  const [text,setText] = useState(name ?? '');
  const next = normalizeRoomName(text);
  const invalid = !!text.trim() && !next;
  const reviewing = status === 'reviewing';
  return <div className="grid min-w-0 gap-1.5">
    <RoomNameField id="mpRoomName" text={text} onText={setText}/>
    <Button type="button" size="sm" disabled={invalid || (reviewing ? next !== null : next === name)}
      onClick={()=>save(next)}>{t('roomName.save')}</Button>
    {status && <span role="status">{t(`roomName.status.${status}`)}</span>}
  </div>;
}
