import {multiplayerConfigForProduct} from '../../../src/contracts/product-catalog.mts';
import type {UiMessageKey} from '../../../src/launcher/i18n.mts';
import type {RoomOwnerSnapshot, MultiplayerRoomService} from '../../services/multiplayer-room';
import {useLocale} from '../../i18n';
import {SettingsSelect} from '../settings/SettingsSelect';

export function RoomLoadoutFields({state, service}: {
  state: RoomOwnerSnapshot; service: Pick<MultiplayerRoomService, 'changeLoadout'>;
}) {
  const {t} = useLocale(), config = multiplayerConfigForProduct(state.product || '');
  const selected = config?.loadouts[state.preferredLoadout]?.selection;
  if (!config?.loadoutFields || !selected || state.spectatorRequested) return null;
  const disabled = !state.connected || !state.room?.synced || state.room.phase !== 'lobby' || state.launchBusy || state.loadoutPending;
  return <div className="mp-loadout-fields">
    {config.loadoutFields.map((field, index) => {
      const choices = new Map<number, string>();
      for (const loadout of config.loadouts) {
        const values = loadout.selection, choice = values?.[index];
        if (!choice || field.dependsOn?.some(dependency => values?.[dependency]?.value !== selected[dependency]?.value)) continue;
        choices.set(choice.value, choice.labelKey);
      }
      return <SettingsSelect key={field.labelKey} id={`mpLoadoutField${index}`} label={t(field.labelKey as UiMessageKey)}
        value={selected[index]?.value} disabled={disabled} onChange={event => {
          const value = Number(event.currentTarget.value);
          const next = config.loadouts.findIndex(loadout => loadout.selection?.every((choice, dimension) =>
            choice.value === (dimension === index ? value : selected[dimension]?.value)));
          if (next >= 0 && next !== state.preferredLoadout) service.changeLoadout(next - state.preferredLoadout);
        }}>
        {[...choices].map(([value, key]) => <option key={value} value={value}>{t(key as UiMessageKey)}</option>)}
      </SettingsSelect>;
    })}
  </div>;
}
