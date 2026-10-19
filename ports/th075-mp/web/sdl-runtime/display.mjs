// Presentation only. Original game coordinates and texture sampling stay in C++.
export function displayRectangle(width, height, pixelRatio, mode = 'integer', compact = false) {
  const dpr = Number.isFinite(pixelRatio) && pixelRatio > 0 ? pixelRatio : 1;
  const availableWidth = Math.max(0, Math.floor(width * dpr + 1e-6));
  const availableHeight = Math.max(0, Math.floor(height * dpr + 1e-6));
  const fit = Math.min(availableWidth / 640, availableHeight / 480);
  const integer = Math.floor(fit + 1e-6);
  // On a small touch screen, a low integer multiplier can make text and gameplay too small.
  const smallFit = compact && integer / fit < .8;
  const scale = mode === 'fit' || fit < 1 || smallFit ? fit : integer;
  const physicalWidth = 640 * scale, physicalHeight = 480 * scale;
  return {
    width: physicalWidth / dpr, height: physicalHeight / dpr,
    left: Math.round((availableWidth - physicalWidth) / 2) / dpr,
    top: Math.round((availableHeight - physicalHeight) / 2) / dpr,
    physicalScale: scale,
  };
}
export function attachDisplay(canvas, getMode) {
  const update = () => {
    const bounds = displayRectangle(innerWidth, innerHeight, devicePixelRatio, getMode(), matchMedia('(pointer:coarse)').matches);
    for (const key of ['width', 'height', 'left', 'top']) canvas.style[key] = bounds[key] + 'px';
  };
  window.addEventListener('resize', update);
  const observer = new MutationObserver(update);
  observer.observe(canvas, { attributes: true, attributeFilter: ['width', 'height'] });
  update();return update;
}
