import fs from 'node:fs';
import path from 'node:path';
export function practiceBuild(root,common=process.env.EAGLER_COMMON_ROOT??path.resolve(root,'../../common')){
  const imgui=path.join(common,'third_party/imgui');
  const sources=['imgui.cpp','imgui_draw.cpp','imgui_tables.cpp','imgui_widgets.cpp','imgui_freetype.cpp'].map(name=>path.join(imgui,name));
  const headers=fs.readdirSync(imgui).filter(name=>/\.(h|inl)$/.test(name)).map(name=>path.join(imgui,name));
  const tools=fs.readdirSync(path.join(common,'include/eagler/thprac')).filter(name=>/\.(hpp|inc)$/.test(name)).map(name=>path.join(common,'include/eagler/thprac',name));
  return {common,sources,inputs:[...sources,...headers,...tools],flags:['-DTH12_ENABLE_THPRAC=1','-DIMGUI_DISABLE_WIN32_FUNCTIONS','-I'+path.join(common,'include'),'-I'+imgui,'--use-port=freetype']};
}
