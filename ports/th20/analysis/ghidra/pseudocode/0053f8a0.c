/* Entry: 0x0053f8a0; symbol: FUN_0053f8a0; body bytes: 488 */

void __thiscall FUN_0053f8a0(void *this,int param_1,undefined4 param_2)

{
  void *pvVar1;
  int *piVar2;
  uint uVar3;
  int iVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  int iVar7;
  uint uVar8;
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
  puStack_c = &LAB_00567ced;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  pvVar1 = (void *)FUN_0040f160((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                 *)this);
  piVar2 = (int *)((int)this + 4);
  piVar9 = (int *)((int)this + 8);
  iVar7 = param_1 - *piVar2 >> 3;
  uVar8 = *piVar9 - *piVar2 >> 3;
  uVar3 = FUN_004162b0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                        *)this);
  if (uVar8 == uVar3) {
    FUN_00414fe0();
  }
  uVar8 = uVar8 + 1;
  local_18 = FUN_00414900(this,uVar8);
  iVar4 = FUN_00413120(pvVar1,&local_18);
  local_20 = iVar4 + 8 + iVar7 * 8;
  local_24 = local_18;
  local_8 = 0;
  local_2c = pvVar1;
  local_28 = iVar4;
  local_1c = local_20;
  uVar5 = move<>(param_2);
  uVar6 = move<>(iVar4 + iVar7 * 8);
  FUN_0053fa90(pvVar1,uVar6,uVar5);
  local_20 = iVar4 + iVar7 * 8;
  if (param_1 == *piVar9) {
    FUN_00413920(*piVar2,*piVar9,iVar4,pvVar1);
  }
  else {
    FUN_00413920(*piVar2,param_1,iVar4,pvVar1);
    local_20 = iVar4;
    FUN_00413920(param_1,*piVar9,iVar4 + 8 + iVar7 * 8,pvVar1);
  }
  local_28 = 0;
  FUN_00414960(this,iVar4,uVar8,local_18);
  local_8 = 0xffffffff;
  FUN_004144e0(&local_2c);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

