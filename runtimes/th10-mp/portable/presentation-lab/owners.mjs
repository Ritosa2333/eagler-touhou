export const OWNER_REGISTRY=Array.from({length:19},(_,id)=>({
  id,label:`ANM layer ${id}`,reviewStatus:'layer-only-unverified',
  authoritativeWriter:'AnmVm update / title owner',endpointPublisher:'AnimationEngine::snapshot_presentation',
  presentationConsumer:'AnimationEngine::present',finalSubmission:'AnmRenderer submission paths',
  identity:'AnmVm.id retained by render-only VM copies',continuity:'field-level native interpolation evidence',
  discontinuity:'script/file/visibility/timer rewind',restoration:'presentation copy, authored VM unchanged',
  fingerprints:[],evidence:['th10_web/cpp/platform/AnimationEngine.cpp','th10_web/cpp/game/AnmRenderer.cpp'],
}));
export const ownerLabel=id=>OWNER_REGISTRY[id]?.label||`ANM layer ${id}`;
