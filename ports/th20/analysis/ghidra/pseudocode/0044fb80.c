/* Entry: 0x0044fb80; symbol: FUN_0044fb80; body bytes: 282 */

void FUN_0044fb80(int param_1)

{
  bool bVar1;
  int iVar2;
  void *this;
  int *piVar3;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *p_Var4;
  int *piVar5;
  int *unaff_FS_OFFSET;
  int local_1c [2];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056863d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  if (param_1 != 0) {
    *(undefined4 *)(param_1 + 0x570) = 1;
    iVar2 = FUN_004494b0((void *)(param_1 + 0x514),1);
    iVar2 = FUN_0040ff90(iVar2);
    if (iVar2 != 0) {
      iVar2 = FUN_004494b0((void *)(param_1 + 0x514),1);
      this = (void *)FUN_0040ff90(iVar2);
      FUN_0040c080(local_1c,8);
      FUN_0044b9f0(this,local_1c);
      local_8 = 0;
      piVar3 = (int *)FUN_00412540();
      while (bVar1 = FUN_00411bb0(local_1c,piVar3), bVar1) {
        p_Var4 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                  *)FUN_0040c300(local_1c);
        piVar5 = (int *)FUN_0040f160(p_Var4);
        FUN_0044fb80(*piVar5);
        FUN_00411c30(local_1c);
      }
      local_8 = 0xffffffff;
      FUN_00411b00(local_1c);
    }
  }
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

