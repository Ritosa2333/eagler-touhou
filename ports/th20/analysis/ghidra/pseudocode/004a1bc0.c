/* Entry: 0x004a1bc0; symbol: FUN_004a1bc0; body bytes: 498 */

void __thiscall FUN_004a1bc0(void *this,int param_1,undefined4 param_2)

{
  void *pvVar1;
  int *piVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  undefined4 uVar7;
  undefined4 uVar8;
  int *piVar9;
  int *unaff_FS_OFFSET;
  void *local_2c;
  int local_28;
  uint local_24;
  int local_20;
  int local_1c;
  uint local_18;
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056962d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  pvVar1 = (void *)FUN_0040f160((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                 *)this);
  piVar2 = (int *)((int)this + 4);
  piVar9 = (int *)((int)this + 8);
  iVar3 = (param_1 - *piVar2) / 0x1c;
  uVar4 = (*piVar9 - *piVar2) / 0x1c;
  uVar5 = FUN_004ab480((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                        *)this);
  if (uVar4 == uVar5) {
    FUN_00414fe0();
  }
  uVar4 = uVar4 + 1;
  local_18 = FUN_004a9850(this,uVar4);
  iVar6 = FUN_004a1730(pvVar1,&local_18);
  local_20 = iVar6 + 0x1c + iVar3 * 0x1c;
  local_24 = local_18;
  local_8 = 0;
  local_2c = pvVar1;
  local_28 = iVar6;
  local_1c = local_20;
  uVar7 = move<>(param_2);
  uVar8 = move<>(iVar3 * 0x1c + iVar6);
  FUN_004a26b0(pvVar1,uVar8,uVar7);
  local_20 = iVar3 * 0x1c + iVar6;
  if (param_1 == *piVar9) {
    FUN_004a22b0(*piVar2,*piVar9,iVar6,pvVar1);
  }
  else {
    FUN_004a22b0(*piVar2,param_1,iVar6,pvVar1);
    local_20 = iVar6;
    FUN_004a22b0(param_1,*piVar9,iVar6 + 0x1c + iVar3 * 0x1c,pvVar1);
  }
  local_28 = 0;
  FUN_004a9960(this,iVar6,uVar4,local_18);
  local_8 = 0xffffffff;
  FUN_004a3de0(&local_2c);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

