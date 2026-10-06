// Included inside title namespace. A read-only binary view of logical ANM poses.
struct AnimationPose {
  float x=0,y=0,z=0,rx=0,ry=0,rotation=0,sx=1,sy=1,u=0,v=0,uvScaleX=1,uvScaleY=1,radius=0,width=0;
  u32 primary=0,secondary=0;int bank=0,sprite=0,layer=0,mode=0,anchorX=0,anchorY=0,blend=0,segments=0;
  u32 flags=0,flags2=0,id=0,owner=0;float alpha=1;u32 reserved[3]{};
};
static_assert(sizeof(AnimationPose)==128);
std::vector<AnimationPose> animationDrawState;
struct AnimationTransform {float x=0,y=0,z=0,sx=1,sy=1,rotation=0;};
AnimationTransform resolveAnimationTransform(const anm_logic::Node& node,int depth=0){
  const auto& s=node.vm.state();AnimationTransform result{s.x+s.engineX+s.offsetX,s.y+s.engineY+s.offsetY,s.z+s.engineZ+s.offsetZ,s.sx,s.sy,s.rotation};
  if(node.parent&&depth<16){auto found=animationScene.nodes().find(node.parent);if(found!=animationScene.nodes().end()){
    const auto parent=resolveAnimationTransform(*found->second,depth+1);result.x+=parent.x;result.y+=parent.y;result.z+=parent.z;
    const auto& immediate=found->second->vm.state();result.sx*=immediate.sx;result.sy*=immediate.sy;
    if(s.mode==1||s.mode==3||s.mode>=14)result.rotation+=immediate.rotation;
  }}return result;
}
void captureAnimationDraw(){
  animationDrawState.clear();std::vector<u32> ids;for(const auto& [id,node]:animationScene.nodes())if(node->alive&&!node->customGeometry)ids.push_back(id);std::sort(ids.begin(),ids.end());
  for(u32 id:ids){const auto& node=*animationScene.nodes().at(id);const auto& s=node.vm.state();const auto transform=resolveAnimationTransform(node);
    animationDrawState.push_back({transform.x+node.viewportX,transform.y+node.viewportY,transform.z,s.rx,s.ry,transform.rotation,transform.sx,transform.sy,s.u,s.v,s.uvScaleX,s.uvScaleY,s.geometryRadius,s.geometryWidth,s.primary,s.secondary,s.bank,s.sprite,s.layer,s.mode,s.anchorX,s.anchorY,s.blend,s.geometryCount,s.flags,s.flags2,id,node.owner,1});
  }
}
