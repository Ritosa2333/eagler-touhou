import {createContext, useState, type CSSProperties, type ReactNode, type RefCallback} from 'react';

/** Controls can compose inside the stage without moving or remounting the iframe. */
export const PlayerStage = createContext<HTMLDivElement | null>(null);
/** Main's one direct player/iframe. It is never keyed by route or product, and
 * touch editing only composes its workbench into this already existing stage. */
export function PlayerSurface({open, editing, replayWorkspace = false, preview = false, previewImage, onElement, onFrame, children}: {
  open: boolean; editing: boolean; replayWorkspace?: boolean; preview?: boolean; previewImage?: string; onElement: RefCallback<HTMLElement>;
  onFrame?: RefCallback<HTMLIFrameElement>; children?: ReactNode;
}) {
  const [stage, setStage] = useState<HTMLDivElement | null>(null);
  return <section ref={onElement} className={`player${open ? ' open' : ''}${replayWorkspace ? ' replay-workspace' : ''}${editing ? ' touch-preview touch-layout-edit touch-enabled' : preview ? ' touch-preview' : ''}`} style={previewImage ? {'--touch-preview-image': previewImage} as CSSProperties : undefined}
    id="player" data-launcher-document="" aria-hidden={!open}>
    <PlayerStage value={stage}>{children}</PlayerStage>
    <div ref={setStage} className="player-stage" style={replayWorkspace ? undefined : {display: 'contents'}}>
      <div className="game-viewport" id="gameViewport"><iframe ref={onFrame} id="gameFrame" title="Touhou game" allow="autoplay; fullscreen" tabIndex={0}/></div>
    </div>
  </section>;
}
