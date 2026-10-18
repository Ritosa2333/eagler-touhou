/* Entry: 0x0044f840; symbol: FUN_0044f840; body bytes: 411 */

void FUN_0044f840(int param_1,void *param_2)

{
  bool bVar1;
  int iVar2;
  void *pvVar3;
  int *piVar4;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *p_Var5;
  int *piVar6;
  int *unaff_FS_OFFSET;
  int local_1c [2];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056867d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  iVar2 = FUN_004494b0((void *)(param_1 + 0x514),1);
  iVar2 = FUN_0040ff90(iVar2);
  if (iVar2 != 0) {
    iVar2 = FUN_004494b0((void *)(param_1 + 0x514),1);
    pvVar3 = (void *)FUN_0040ff90(iVar2);
    FUN_0040c080(local_1c,8);
    FUN_0044b9f0(pvVar3,local_1c);
    local_8 = 0;
    piVar4 = (int *)FUN_00412540();
    while( true ) {
      bVar1 = FUN_00411bb0(local_1c,piVar4);
      if (!bVar1) break;
      p_Var5 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)FUN_0040c300(local_1c);
      pvVar3 = param_2;
      piVar6 = (int *)FUN_0040f160(p_Var5);
      FUN_0044f840(*piVar6,pvVar3);
      FUN_00411c30(local_1c);
    }
    local_8 = 0xffffffff;
    FUN_00411b00(local_1c);
  }
  if (*(int *)(param_1 + 0x570) != 2) {
    FUN_0044a670((void *)(param_1 + 0x53c),param_1);
    FUN_00411ea0(param_2,(void *)(param_1 + 0x53c));
  }
  *(undefined4 *)(param_1 + 0x570) = 2;
  *(undefined4 *)(param_1 + 0x558) = 0;
  *(undefined4 *)(param_1 + 0x55c) = 0;
  *(undefined4 *)(param_1 + 0x554) = 0;
  *(undefined4 *)(param_1 + 0x550) = 0;
  iVar2 = FUN_004494b0((void *)(param_1 + 0x514),0);
  FUN_00411ce0(iVar2);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

