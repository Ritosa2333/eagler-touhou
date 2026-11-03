import {CircleHelp, LogOut, Maximize, Minimize, RotateCw} from 'lucide-react';

const iconProps = {className: 'touch-utility-icon', 'aria-hidden': true, focusable: false, style: {fill: 'none'}} as const;

export function FullscreenIcons() {
  return <><span className="fullscreen-enter-icon"><Maximize {...iconProps}/></span><span className="fullscreen-exit-icon"><Minimize {...iconProps}/></span></>;
}
export function PlaybackExitIcon() {return <LogOut {...iconProps}/>;}
export function TouchHelpIcon() {return <CircleHelp {...iconProps}/>;}
export function OrientationIcon() {return <RotateCw {...iconProps}/>;}
