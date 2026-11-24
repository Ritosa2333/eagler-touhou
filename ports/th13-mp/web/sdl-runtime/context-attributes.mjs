// WebGL 上下文属性（SP/MP shell 与独立 th13.html 共用，两侧逐字节相同）。
// SDL3 经 Emscripten 创建 WebGL2 上下文时只传入它认识的属性；在模块初始化前
// 包一层 canvas.getContext，为 webgl/webgl2 追加 powerPreference:
// 'high-performance'——双显卡机器选独显。不支持的浏览器忽略该提示。
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
