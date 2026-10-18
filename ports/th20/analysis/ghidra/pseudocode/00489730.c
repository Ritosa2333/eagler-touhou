/* Entry: 0x00489730; symbol: FUN_00489730; body bytes: 253 */

void __thiscall FUN_00489730(void *this,uint param_1,undefined4 param_2,undefined4 param_3)

{
  undefined4 uVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  undefined4 *puVar4;
  int *unaff_FS_OFFSET;
  void *local_1c;
  undefined1 local_16;
  undefined1 local_15;
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_005692fd;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  uVar3 = FUN_0040f160((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                        *)this);
  local_15 = 0;
  FUN_0040de90(&local_16);
  if (param_1 != 0) {
    FUN_00497600(this,param_1);
    local_8 = 0;
    uVar1 = *(undefined4 *)((int)this + 4);
    local_1c = this;
    puVar4 = (undefined4 *)move<>(param_3);
    uVar2 = *puVar4;
    puVar4 = (undefined4 *)move<>(param_2);
    uVar3 = FUN_0048a370(*puVar4,uVar2,uVar1,uVar3);
    *(undefined4 *)((int)this + 8) = uVar3;
    local_1c = (void *)0x0;
    local_8 = 0xffffffff;
    FUN_0048b810((int *)&local_1c);
  }
  FUN_0040e5e0();
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

