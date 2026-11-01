import type { UiMessageKey } from '../../../src/launcher/i18n.mts';
import type { keyboardActionCodes } from '../../../src/launcher/keyboard-preferences.mts';
import { useLocale } from '../../i18n';
import { Button } from '../ui/button';
export type InputAction = keyof typeof keyboardActionCodes;
export const inputActionOrder: readonly InputAction[] = [
  'up',
  'down',
  'left',
  'right',
  'shoot',
  'bomb',
  'focus',
  'action',
  'pause',
  'confirm',
  'skip',
  'restart',
];

/** Keyboard and controller use the same familiar action/binding table. */
export function InputBindingsTable({
  bindings,
  learning,
  onBind,
  onReset,
  resetLabel,
  isActive,
  actions = inputActionOrder,
  names,
}: {
  bindings: Record<InputAction, readonly string[]>;
  learning: InputAction | null;
  onBind(action: InputAction): void;
  onReset(action: InputAction): void;
  resetLabel: string;
  isActive?(action: InputAction): boolean;
  actions?: readonly InputAction[];
  names?: Partial<Record<InputAction, UiMessageKey>>;
}) {
  const { t } = useLocale();
  return (
    <table className="input-binding-table">
      <thead>
        <tr>
          <th scope="col">{t('input.text.action')}</th>
          <th scope="col">{t('input.text.binding')}</th>
          <th scope="col">
            <span className="input-visually-hidden">{resetLabel}</span>
          </th>
        </tr>
      </thead>
      <tbody>
        {actions.map((action) => {
          const name = t(names?.[action] ?? `input.action.${action}`),
            keys = bindings[action],
            waiting = learning === action;
          return (
            <tr
              key={action}
              data-action={action}
              data-learning={waiting}
              data-active={!!isActive?.(action)}
              className={action === 'shoot' || action === 'pause' ? 'input-action-divider' : undefined}
            >
              <th scope="row">{name}</th>
              <td>
                <Button
                  variant="legacy"
                  size="legacy"
                  type="button"
                  className="input-binding-button"
                  aria-label={
                    waiting
                      ? t('input.bindingListening', { name, hint: t('input.text.waiting_for_input_click_to_cancel') })
                      : t('input.bindingChange', {
                          name,
                          keys: keys.join(' / ') || t('input.text.unbound'),
                          hint: t('input.text.change_binding'),
                        })
                  }
                  aria-pressed={waiting}
                  onClick={() => onBind(action)}
                  onKeyDown={(event) => {
                    if (!waiting && event.key === 'Delete') {
                      event.preventDefault();
                      onReset(action);
                    }
                  }}
                >
                  {waiting ? (
                    <span className="input-binding-listening">{t('input.text.press_a_control')}</span>
                  ) : keys.length ? (
                    keys.map((key, n) => (
                      <kbd key={n} className={n ? 'input-keycap secondary' : 'input-keycap'}>
                        {key}
                      </kbd>
                    ))
                  ) : (
                    <span className="input-binding-empty">—</span>
                  )}
                </Button>
              </td>
              <td>
                <Button
                  variant="legacy"
                  size="legacy"
                  type="button"
                  className="input-binding-reset"
                  title={`${resetLabel} · ${name}`}
                  aria-label={`${resetLabel} · ${name}`}
                  onClick={() => onReset(action)}
                >
                  ↶
                </Button>
              </td>
            </tr>
          );
        })}
      </tbody>
    </table>
  );
}
