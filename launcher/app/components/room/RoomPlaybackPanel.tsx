import { type ReactNode, useLayoutEffect, useRef, useState } from 'react';

/** Measure the controls' natural height so the chat yields space continuously,
 * including the native th09 panel and its own expand/collapse changes. */
export function RoomPlaybackPanel({ workspace, children }: { workspace: HTMLElement; children: ReactNode }) {
  const content = useRef<HTMLDivElement>(null);
  const [height, setHeight] = useState(0);
  useLayoutEffect(() => {
    const element = content.current;
    if (!element) return;
    const measure = () => {
      const next = element.offsetHeight;
      setHeight((previous) => (previous === next ? previous : next));
      workspace.style.setProperty('--room-playback-height', `${next ? next + 12 : 0}px`);
    };
    const observer = new ResizeObserver(measure);
    observer.observe(element);
    measure();
    return () => {
      observer.disconnect();
      workspace.style.removeProperty('--room-playback-height');
    };
  }, [workspace]);
  return (
    <div
      className="room-workspace-playback"
      data-visible={height > 0}
      style={{ height }}
      inert={height === 0}
      aria-hidden={height === 0}
    >
      <div ref={content} className="room-workspace-playback-content">
        {children}
      </div>
    </div>
  );
}
