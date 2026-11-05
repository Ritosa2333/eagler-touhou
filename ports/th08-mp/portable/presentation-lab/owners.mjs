// TH08 owns these semantics. The common lab only consumes normalized labels
// and policy evidence; numeric owner IDs are part of the TH08 native ABI.
export const OWNERS=['未标记','标题菜单','菜单说明','暂停/重试','重试','敌弹','激光','特效','敌人/使魔','道具','HUD','3D背景','符卡UI','自机','自机弹','ASCII','自机Bomb','自机Option','ASCII文本','分数弹字','时间弹字'];
export const STATE_GROUPS=['RNG/资源数值','自机逻辑','弹幕逻辑','激光逻辑','敌人逻辑','特效逻辑','标题VM','场景/逻辑计数','背景镜头'];
export const WORLD_OWNER_IDS=new Set([5,6,7,8,9,10,11,13,14,15,16,17,19,20]);
export const MOTION_OWNER_IDS=new Set([5,6,8,9,13,14,16,17,19]);
export const SOURCE_PATHS={1:'TitleSupport.cpp / TitleView.cpp',2:'TitleView.cpp',3:'UiMenus.cpp',4:'UiMenus.cpp',5:'BulletDrawing.cpp',6:'BulletDrawing.cpp',7:'EffectSystem.cpp',8:'EnemyDrawing.cpp',9:'ItemPool.cpp',10:'GuiController.cpp / GuiView.cpp',11:'BackgroundObjects.cpp / BackgroundView.cpp',12:'SpellDrawing.cpp',13:'PlayerSimulation.cpp',14:'PlayerShotDraw.cpp',16:'PlayerBomb*.cpp'};

// Entries remain explicitly incomplete until the source anchors and lifecycle
// rules have been reviewed against the current title revision. UI labels alone
// are never treated as proof that an owner is correctly connected.
export const OWNER_REGISTRY=OWNERS.map((label,id)=>({
  id,label,source:SOURCE_PATHS[id]||null,reviewStatus:id?'legacy-unverified':'unscoped',
  authoritativeWriter:null,endpointPublisher:null,presentationConsumer:null,
  finalSubmission:null,identity:null,continuity:null,discontinuity:null,
  restoration:null,fingerprints:[],evidence:[],
}));
