import {useSyncExternalStore} from 'react';
import {useReducedMotion} from 'motion/react';

function subscribe(listener: () => void) {
  const observer = new MutationObserver(listener);
  observer.observe(document.body, {attributes:true, attributeFilter:['class']});
  return () => observer.disconnect();
}

function siteReducedMotion() {
  return document.body.classList.contains('less-motion');
}

export function useSiteReducedMotion() {
  const systemReduced = useReducedMotion();
  const siteReduced = useSyncExternalStore(subscribe, siteReducedMotion, () => false);
  return !!systemReduced || siteReduced;
}
