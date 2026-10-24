// TH11 owner IDs are part of the native ABI: GameSession::draw assigns one
// scope per scene pass. IDs 12+layer identify ANM draw layers. UI labels are
// not proof of coverage; reviewStatus stays explicit until audited.
export const OWNERS = [
  '未标记', '舞台背景', '舞台前景', '自机', '道具', '激光', '敌弹', '符卡',
  'HUD', 'ASCII', '分数弹字', '渲染合成', ...Array.from({length:31},(_,i)=>`ANM layer ${i}`),
];
export const ownerLabel=id=>OWNERS[id]||`owner ${id}`;
export const STATE_GROUPS=[
  '会话/经济','自机逻辑','敌弹逻辑','道具逻辑','激光逻辑','敌人逻辑','ANM 注册表','场景相机','RNG/Replay 游标',
];
export const WORLD_OWNER_IDS=new Set([1,2,3,4,5,6,7,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42]);
export const OWNER_REGISTRY=OWNERS.map((label,id)=>({
  id,label,reviewStatus:id>=12?'layer-only-unverified':id?'owner-scope-unverified':'unscoped',
  authoritativeWriter:'GameSession::update / owner update',
  endpointPublisher:'GameSession::update (presentation::begin_tick)',
  presentationConsumer:'AnmRenderer::draw field presentation',
  finalSubmission:'AnmRenderer::submit_quad / geometry submission',
  identity:'AnmVm.id retained by render-only copies',
  continuity:'field-level ANM continuity + owner recomputation',
  discontinuity:'script/file/visibility/timer rewind; 128px teleport gate',
  restoration:'presentation copy; authoritative VM unchanged',
  fingerprints:[],evidence:['web/cpp/game/AnmRenderer.cpp','web/cpp/game/PresentationAudit.cpp'],
}));
