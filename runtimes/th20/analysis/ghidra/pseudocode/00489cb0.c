/* Entry: 0x00489cb0; symbol: FUN_00489cb0; body bytes: 240 */

void __thiscall FUN_00489cb0(void *this,uint *param_1)

{
  int iVar1;
  int iVar2;
  void *pvVar3;
  int iVar4;
  int *unaff_FS_OFFSET;
  void *local_20;
  int local_1c;
  uint local_18;
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056933d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  pvVar3 = (void *)FUN_0040f160((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                 *)this);
  iVar1 = *(int *)((int)this + 8);
  iVar2 = *(int *)((int)this + 4);
  iVar4 = FUN_00489390(pvVar3,param_1);
  local_18 = *param_1;
  local_8 = 0;
  local_20 = pvVar3;
  local_1c = iVar4;
  FUN_0048a450(*(int *)((int)this + 4),*(int *)((int)this + 8),iVar4,pvVar3);
  local_1c = 0;
  FUN_004976f0(this,iVar4,iVar1 - iVar2 >> 2,*param_1);
  local_8 = 0xffffffff;
  FUN_0048b9e0(&local_20);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

