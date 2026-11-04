import {useRef, type ComponentPropsWithoutRef} from 'react';
import {Slider as Primitive} from 'radix-ui';
import {cn} from './utils';

type SliderProps = Omit<ComponentPropsWithoutRef<typeof Primitive.Root>, 'value' | 'defaultValue' | 'onValueChange' | 'onValueCommit'> & {
  value: number;
  onValueChange(value: number): void;
  onValueCommit?(value: number): void;
  onValueCancel?(): void;
  'aria-label': string;
  'aria-valuetext'?: string;
};
/** Preview, release and cancellation stay distinct for settings and playback. */
export function Slider({value, onValueChange, onValueCommit, onValueCancel, className, 'aria-label': label, 'aria-valuetext': valueText, ...props}: SliderProps) {
  const interaction = useRef<{cancelled: boolean; start?: number; changed?: number; committed?: number}>({cancelled: false});
  const change = (next: number) => {
    if (interaction.current.cancelled || interaction.current.committed === next) return;
    interaction.current.changed = next;
    onValueChange(next);
  };
  const commit = (next: number) => {
    if (interaction.current.cancelled) return;
    // Radix may commit a keyboard update before notifying its value change.
    // Consumers always receive preview -> commit, once for each adjustment.
    if (interaction.current.changed !== next) change(next);
    interaction.current.committed = next;
    onValueCommit?.(next);
  };
  return <Primitive.Root {...props} data-slot="slider" className={cn('ui-kit-slider', className)} value={[value]}
    onValueChange={([next]) => change(next)} onValueCommit={([next]) => commit(next)}
    onPointerDownCapture={event => {interaction.current = {cancelled: false, start:value};props.onPointerDownCapture?.(event);}}
    onPointerUp={event => {
      props.onPointerUp?.(event);
      // Returning to the starting value produces no Radix commit. Release any
      // temporary preview instead of letting it hide later controlled updates.
      const current = interaction.current;
      if (!current.cancelled && current.changed !== undefined && current.changed === current.start) {
        current.cancelled = true; onValueCancel?.();
      }
    }}
    onKeyDownCapture={event => {interaction.current = {cancelled: false};props.onKeyDownCapture?.(event);}}
    onPointerCancelCapture={event => {interaction.current.cancelled = true;onValueCancel?.();props.onPointerCancelCapture?.(event);}}>
    <Primitive.Track className="ui-kit-slider-track"><Primitive.Range className="ui-kit-slider-range"/></Primitive.Track>
    <Primitive.Thumb className="ui-kit-slider-thumb" aria-label={label} aria-valuetext={valueText} aria-describedby={props['aria-describedby']}/>
  </Primitive.Root>;
}
