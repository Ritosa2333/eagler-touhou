import {useLayoutEffect, useRef, useState, type HTMLAttributes} from 'react';
import {cn} from './utils';
import './overflow-text.css';

/** A long public name stays on one line and can be panned or keyboard-scrolled. */
export function OverflowText({children, className,align='start',autoScroll=false, ...props}: Omit<HTMLAttributes<HTMLSpanElement>, 'children'> & {children: string;align?:'start'|'end'|'center';autoScroll?:boolean}) {
  const ref = useRef<HTMLSpanElement>(null), [overflow, setOverflow] = useState(false);
  useLayoutEffect(() => {
    const node = ref.current;
    if (!node) return;
    const label = node.firstElementChild;
    const measure = () => {
      const width = autoScroll && label ? label.getBoundingClientRect().width : node.scrollWidth;
      const distance = width - node.clientWidth;
      setOverflow(children.length > 0 && distance > 1);
      if (autoScroll) {
        node.style.setProperty('--ui-overflow-distance', distance + 'px');
        node.style.setProperty('--ui-overflow-duration', Math.max(3, 1.5 + distance / 28) + 's');
      }
    };
    node.scrollLeft = 0;
    measure();
    if (typeof ResizeObserver === 'undefined') return;
    const observer = new ResizeObserver(measure); observer.observe(node);if(label)observer.observe(label);
    return () => observer.disconnect();
  }, [children, autoScroll]);
  return <span {...props} ref={ref} className={cn('ui-overflow-text', className)} data-align={align} data-auto-scroll={autoScroll && overflow || undefined} title={children} tabIndex={overflow ? 0 : undefined}><span className="ui-overflow-text-label">{children}</span></span>;
}
