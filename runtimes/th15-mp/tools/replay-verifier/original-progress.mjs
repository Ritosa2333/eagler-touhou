// Player mode is global 0x4e7794. Scene flags belong to the scene owner.
export const sceneFlags=(m,scene)=>m.u32(scene+0x90);
export const stageCleared=(m,scene,stage)=>(sceneFlags(m,scene)&0x4000)!==0||m.u32(0x4e73f0)===stage+1;
