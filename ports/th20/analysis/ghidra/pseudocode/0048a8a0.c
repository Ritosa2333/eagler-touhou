/* Entry: 0x0048a8a0; symbol: FUN_0048a8a0; body bytes: 236 */

void __cdecl
FUN_0048a8a0(_Ptr_base<struct__EXCEPTION_RECORD_const_> *param_1,undefined4 param_2,
            undefined4 param_3)

{
  undefined4 uVar1;
  undefined4 uVar2;
  int *unaff_FS_OFFSET;
  undefined1 local_20 [4];
  int local_1c;
  undefined1 local_15;
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056904d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_0040b120(&local_15);
  FUN_00413f50(local_20,&local_15);
  local_8 = 0;
  FUN_004842f0((int)local_20);
  uVar1 = move<>(param_3);
  FUN_00489680(local_1c,param_2,uVar1);
  FUN_0040c080(param_1,8);
  FUN_0047b880(param_1);
  uVar1 = move<>(local_1c + 0xc);
  uVar2 = FUN_00484720((int)local_20);
  uVar2 = move<>(uVar2);
  FUN_0047b030(param_1,uVar1,uVar2);
  local_8 = 0xffffffff;
  FUN_0047c220((int)local_20);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

