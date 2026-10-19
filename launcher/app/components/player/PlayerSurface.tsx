import {createContext, useState, type CSSProperties, type ReactNode, type RefCallback} from 'react';
import {useWorkspaceViewport} from './use-workspace-viewport';
import './workspace-viewport.css';

/** Controls can compose inside the stage without moving or remounting the iframe. */
export const PlayerStage = createContext<HTMLDivElement | null>(null);
/** Main's one direct player/iframe. It is never keyed by route or product, and
 * touch editing only composes its workbench into this already existing stage. */
export function PlayerSurface({open, editing, replayWorkspace = false, roomWorkspace = false, preview = false, previewImage, onElement, onFrame, children}: {
  open: boolean; editing: boolean; replayWorkspace?: boolean; roomWorkspace?: boolean; preview?: boolean; previewImage?: string; onElement: RefCallback<HTMLElement>;
  onFrame?: RefCallback<HTMLIFrameElement>; children?: ReactNode;
}) {
  const [stage, setStage] = useState<HTMLDivElement | null>(null);
  useWorkspaceViewport(stage, roomWorkspace || replayWorkspace);
  return <section ref={onElement} className={`player${open ? ' open' : ''}${replayWorkspace ? ' replay-workspace' : ''}${roomWorkspace ? ' room-workspace' : ''}${editing ? ' touch-preview touch-layout-edit touch-enabled' : preview ? ' touch-preview' : ''}`} style={previewImage ? {'--touch-preview-image': previewImage} as CSSProperties : undefined}
    id="player" data-launcher-document="" aria-hidden={!open}>
    <PlayerStage value={stage}>{children}</PlayerStage>
    <div ref={setStage} className="player-stage" style={replayWorkspace || roomWorkspace ? undefined : {display: 'contents'}}>
      <div className="game-viewport" id="gameViewport"><iframe ref={onFrame} id="gameFrame" title="Touhou game" allow="autoplay; fullscreen" tabIndex={roomWorkspace ? -1 : 0} inert={roomWorkspace}/></div>
    </div>
  </section>;
}
