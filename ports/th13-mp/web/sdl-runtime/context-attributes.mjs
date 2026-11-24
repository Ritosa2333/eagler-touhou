const contextAttributes = {powerPreference: 'high-performance'};

export function installContextAttributes(canvas) {
  if (!canvas || canvas.__eaglerContextAttributes) return;
  const getContext = canvas.getContext.bind(canvas);
  canvas.getContext = (type, attributes) => {
    const webgl = type === 'webgl2' || type === 'webgl';
    return getContext(type, webgl ? {...attributes, ...contextAttributes} : attributes);
  };
  canvas.__eaglerContextAttributes = true;
}
