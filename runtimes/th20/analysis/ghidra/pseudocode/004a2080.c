/* Entry: 0x004a2080; symbol: FUN_004a2080; body bytes: 334 */

void __thiscall FUN_004a2080(void *this,uint param_1)

{
  uint uVar1;
  void *pvVar2;
  int iVar3;
  int iVar4;
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
  puStack_c = &LAB_0056966d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  uVar1 = FUN_004ab440((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                        *)this);
  if (uVar1 < param_1) {
    FUN_00414fe0();
  }
  pvVar2 = (void *)FUN_0040f160((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                 *)this);
  iVar3 = (*(int *)((int)this + 8) - *(int *)((int)this + 4)) / 0x88;
  local_18 = FUN_004a97f0(this,param_1);
  iVar4 = FUN_004a1710(pvVar2,&local_18);
  local_20 = iVar3 * 0x88 + iVar4;
  local_24 = local_18;
  local_8 = 0;
  local_2c = pvVar2;
  local_28 = iVar4;
  local_1c = local_20;
  local_1c = FUN_004a2380(local_20,param_1 - iVar3,pvVar2);
  FUN_004a21e0(*(int *)((int)this + 4),*(int *)((int)this + 8),iVar4,pvVar2);
  local_28 = 0;
  FUN_004a98b0(this,iVar4,param_1,local_18);
  local_8 = 0xffffffff;
  FUN_004a3d80(&local_2c);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

