/* Entry: 0x0044a090; symbol: FUN_0044a090; body bytes: 382 */

void __thiscall FUN_0044a090(void *this,int param_1)

{
  int iVar1;
  bool bVar2;
  int *piVar3;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *p_Var4;
  int *piVar5;
  undefined4 *puVar6;
  int *unaff_FS_OFFSET;
  int local_20;
  int local_1c [2];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_005683cd;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_0044a640((void *)((int)this + 0x30),0);
  FUN_0040c080(local_1c,8);
  FUN_00412280(this,local_1c);
  local_8 = 0;
  piVar3 = (int *)FUN_00412540();
  while( true ) {
    bVar2 = FUN_00411bb0(local_1c,piVar3);
    if (!bVar2) break;
    p_Var4 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
              *)FUN_0040c300(local_1c);
    piVar5 = (int *)FUN_0040f160(p_Var4);
    if (*(int *)(*piVar5 + 0x570) == 0) {
      piVar5 = (int *)FUN_0040f160(p_Var4);
      local_20 = *(int *)(*piVar5 + 0x14);
      if ((local_20 < 0x2e) || (0x35 < local_20)) {
        if (local_20 == 0x2d) {
          local_20 = 0x1a;
        }
      }
      else {
        local_20 = local_20 + -0x11;
      }
      if (local_20 == param_1) {
        piVar5 = (int *)FUN_0040f160(p_Var4);
        iVar1 = *piVar5;
        puVar6 = (undefined4 *)FUN_0040f160(p_Var4);
        FUN_0044a670((void *)(iVar1 + 0x500),*puVar6);
        piVar5 = (int *)FUN_0040f160(p_Var4);
        FUN_00411ea0((void *)((int)this + 0x30),(void *)(*piVar5 + 0x500));
      }
    }
    FUN_00411c30(local_1c);
  }
  local_8 = 0xffffffff;
  FUN_00411b00(local_1c);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

