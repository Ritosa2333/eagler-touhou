import {useLayoutEffect, useRef, useState, type HTMLAttributes} from 'react';
import {cn} from './utils';
import './overflow-text.css';

/** A long public name stays on one line and can be panned or keyboard-scrolled. */
export function OverflowText({children, className,align='start', ...props}: Omit<HTMLAttributes<HTMLSpanElement>, 'children'> & {children: string;align?:'start'|'end'|'center'}) {
  const ref = useRef<HTMLSpanElement>(null), [overflow, setOverflow] = useState(false);
  useLayoutEffect(() => {
    const node = ref.current;
    if (!node) return;
    const measure = () => setOverflow(children.length > 0 && node.scrollWidth > node.clientWidth + 1);
    node.scrollLeft = 0;
    measure();
    if (typeof ResizeObserver === 'undefined') return;
    const observer = new ResizeObserver(measure); observer.observe(node);if(node.firstElementChild)observer.observe(node.firstElementChild);
    return () => observer.disconnect();
  }, [children]);
  return <span {...props} ref={ref} className={cn('ui-overflow-text', className)} data-align={align} title={children} tabIndex={overflow ? 0 : undefined}><span className="ui-overflow-text-label">{children}</span></span>;
}
