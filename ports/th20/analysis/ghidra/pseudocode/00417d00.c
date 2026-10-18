/* Entry: 0x00417d00; symbol: FUN_00417d00; body bytes: 182 */

void __thiscall FUN_00417d00(void *this,undefined4 *param_1,undefined4 param_2)

{
  undefined4 uVar1;
  uint uVar2;
  uint *puVar3;
  int *unaff_FS_OFFSET;
  byte local_1d;
  undefined4 local_1c;
  undefined4 local_18;
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00567ead;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  FID_conflict__Compressed_pair<class_std::allocator<char>,class_std::_String_val<struct_std::_Simple_types<char>_>,1><class_std::allocator<char>_>
            (this,(uint)local_1d,param_2);
  local_8 = 0;
  FUN_0040c080(&local_1c,8);
  local_1c = *param_1;
  local_18 = param_1[1];
  uVar1 = FUN_0040ff90((int)&local_1c);
  uVar2 = move<>(uVar1);
  puVar3 = (uint *)FUN_0040c300(&local_1c);
  FUN_00417ec0(this,puVar3,uVar2);
  local_8 = 0xffffffff;
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

