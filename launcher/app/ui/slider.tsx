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
  const cancelled = useRef(false);
  return <Primitive.Root {...props} data-slot="slider" className={cn('ui-kit-slider', className)} value={[value]}
    onValueChange={([next]) => onValueChange(next)} onValueCommit={([next]) => {if (!cancelled.current) onValueCommit?.(next);}}
    onPointerDownCapture={event => {cancelled.current = false;props.onPointerDownCapture?.(event);}}
    onKeyDownCapture={event => {cancelled.current = false;props.onKeyDownCapture?.(event);}}
    onPointerCancelCapture={event => {cancelled.current = true;onValueCancel?.();props.onPointerCancelCapture?.(event);}}>
    <Primitive.Track className="ui-kit-slider-track"><Primitive.Range className="ui-kit-slider-range"/></Primitive.Track>
    <Primitive.Thumb className="ui-kit-slider-thumb" aria-label={label} aria-valuetext={valueText} aria-describedby={props['aria-describedby']}/>
  </Primitive.Root>;
}
