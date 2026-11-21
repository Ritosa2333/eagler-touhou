// 低延迟画布（SP/MP shell 与独立 th13.html 共用）。
// SDL3 经 Emscripten 创建 WebGL2 上下文时只传入它认识的属性；在模块
// 初始化前包一层 canvas.getContext，为 webgl/webgl2 追加：
//   desynchronized: true            —— 支持的平台上让画布绕过合成器排队，
//                                       每帧提前一个刷新周期上屏；
//   powerPreference:'high-performance' —— 双显卡机器选独显。
// 不支持的浏览器忽略这两个提示，走原路径。实际授予情况经 presentation()
// 查询（getContextAttributes），由 runtime-info / window 暴露上报。
const lowLatencyAttributes = {desynchronized: true, powerPreference: 'high-performance'};

export function installLowLatencyCanvas(canvas) {
  if (!canvas || canvas.__eaglerLowLatencyCanvas) return canvas?.__eaglerLowLatencyCanvas ?? null;
  let webglContext = null;
  const getContext = canvas.getContext.bind(canvas);
  canvas.getContext = (type, attributes) => {
    const webgl = type === 'webgl2' || type === 'webgl';
    const context = getContext(type, webgl ? {...attributes, ...lowLatencyAttributes} : attributes);
    if (webgl && context) webglContext = context;
    return context;
  };
  const presentation = () => {
    const attributes = webglContext?.getContextAttributes?.() ?? null;
    return {desynchronized: attributes?.desynchronized === true,
            powerPreference: attributes?.powerPreference ?? null};
  };
  // “SDL3 / WebGL2 (desynchronized) / C++”：与 runtime-info 的 renderer 字段同式。
  const rendererReport = suffix => 'SDL3 / WebGL2' +
    (presentation().desynchronized ? ' (desynchronized)' : '') + (suffix ?? ' / C++');
  return canvas.__eaglerLowLatencyCanvas = {presentation, rendererReport};
}
