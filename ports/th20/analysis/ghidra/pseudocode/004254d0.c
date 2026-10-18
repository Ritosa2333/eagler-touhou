/* Entry: 0x004254d0; symbol: FUN_004254d0; body bytes: 328 */

void __thiscall FUN_004254d0(void *this,uint param_1)

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
  puStack_c = &LAB_0056814d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  uVar1 = FUN_00429000((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                        *)this);
  if (uVar1 < param_1) {
    FUN_00414fe0();
  }
  pvVar2 = (void *)FUN_0040f160((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                 *)this);
  iVar4 = *(int *)((int)this + 8) - *(int *)((int)this + 4) >> 4;
  local_18 = FUN_00428a90(this,param_1);
  iVar3 = FUN_004251e0(pvVar2,&local_18);
  local_20 = iVar4 * 0x10 + iVar3;
  local_24 = local_18;
  local_8 = 0;
  local_2c = pvVar2;
  local_28 = iVar3;
  local_1c = local_20;
  local_1c = FUN_00425710(local_20,param_1 - iVar4,pvVar2);
  FUN_00425640(*(int *)((int)this + 4),*(int *)((int)this + 8),iVar3,pvVar2);
  local_28 = 0;
  FUN_00428af0(this,iVar3,param_1,local_18);
  local_8 = 0xffffffff;
  FUN_00426050(&local_2c);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

