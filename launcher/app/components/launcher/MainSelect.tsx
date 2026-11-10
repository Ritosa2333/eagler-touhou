import { createContext, forwardRef, useContext, useLayoutEffect, useRef, useState, useCallback, type ReactNode, type Ref, type SelectHTMLAttributes } from 'react';
import { useLocale } from '../../i18n';
import { usePopupPadding } from '../../ui/use-popup-padding';
import { Select, SelectContent, SelectItem, SelectTrigger, SelectValue, closeSelectMenus } from '../../ui/select';

export type MainSelectProps = SelectHTMLAttributes<HTMLSelectElement> & {
  'data-trigger-i18n'?: string;
  renderPicker?(change: (value: string) => void, ref: Ref<HTMLButtonElement>): ReactNode;
};
const Prefix = createContext<ReactNode>(null);
const emptyValue = '__eagler_empty_selection__';

export function MainSelectPrefix({ children, prefix }: { children: ReactNode; prefix: ReactNode }) {
  return <Prefix.Provider value={prefix}>{children}</Prefix.Provider>;
}

export const closeMainSelectMenus = closeSelectMenus;

/** The native element owns forms, refs and change events; shared Select owns the popup. */
export const MainSelect = forwardRef<HTMLSelectElement, MainSelectProps>(function MainSelect({renderPicker, ...props}, ref) {
  const { t } = useLocale();
  const native = useRef<HTMLSelectElement | null>(null);
  const trigger = useRef<HTMLButtonElement>(null);
  const prefix = useContext(Prefix);
  const [open, setOpen] = useState(false);
  const padding = usePopupPadding(open);
  const [value, setValue] = useState(String(props.value ?? props.defaultValue ?? ''));
  const [options, setOptions] = useState<{ value: string; label: string; disabled: boolean }[]>([]);
  const attach = useCallback((element: HTMLSelectElement | null) => {
    native.current = element;
    if (typeof ref === 'function') ref(element);
    else if (ref) ref.current = element;
  }, [ref]);

  useLayoutEffect(() => {
    if (!native.current) return;
    const next = [...native.current.options]
      .filter((option) => !option.hidden)
      .map((option) => ({ value: option.value, label: option.text, disabled: option.disabled }));
    setOptions((previous) => (JSON.stringify(previous) === JSON.stringify(next) ? previous : next));
    setValue(native.current.value);
  });

  const change = (next: string) => {
    const select = native.current;
    if (!select) return;
    select.value = next === emptyValue ? '' : next;
    setValue(select.value);
    select.dispatchEvent(new Event('change', { bubbles: true }));
  };

  return (
    <div className="ui-select mizuki-select">
      <select
        {...props}
        ref={attach}
        className={`${props.className ?? ''} custom-select-native`}
        tabIndex={-1}
        aria-hidden="true"
        onFocus={(event) => {
          props.onFocus?.(event);
          trigger.current?.focus();
        }}
      >
        {props.children}
      </select>
      {renderPicker ? renderPicker(change, trigger) : <Select
        value={value || emptyValue}
        onValueChange={change}
        open={open}
        onOpenChange={setOpen}
        disabled={props.disabled}
        required={props.required}
      >
        <SelectTrigger
          ref={trigger}
          id={props.id ? `${props.id}-trigger` : undefined}
          className="mizuki-select-trigger"
          aria-label={props['aria-label'] || (props['aria-labelledby'] ? undefined : t('common.selectOption'))}
          aria-labelledby={props['aria-labelledby']}
          aria-describedby={props['aria-describedby']}
          title={props.title}
        >
          {prefix}
          <SelectValue>{options.find((option) => option.value === value)?.label ?? ''}</SelectValue>
        </SelectTrigger>
        <SelectContent
          sideOffset={5}
          collisionPadding={padding}
          data-launcher-document={native.current?.closest('[data-launcher-document]') ? '' : undefined}
        >
          {options.map((option) => (
            <SelectItem
              key={option.value || emptyValue}
              value={option.value || emptyValue}
              disabled={option.disabled}
              data-value={option.value}
            >
              {option.label}
            </SelectItem>
          ))}
        </SelectContent>
      </Select>}
    </div>
  );
});
