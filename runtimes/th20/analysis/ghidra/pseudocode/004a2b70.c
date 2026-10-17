/* Entry: 0x004a2b70; symbol: FUN_004a2b70; body bytes: 162 */

void * __cdecl FUN_004a2b70(void *param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  undefined4 uVar2;
  int *unaff_FS_OFFSET;
  undefined1 local_24 [8];
  undefined1 local_1c [4];
  void *local_18;
  void *local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00568060;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  iVar1 = move<>(param_3);
  uVar2 = FUN_0048b160(iVar1);
  local_14 = FUN_004a2c20(local_24,param_2,uVar2);
  uVar2 = move<>(param_3);
  uVar2 = FUN_0048b180(uVar2);
  local_18 = FUN_00413d50(local_1c,param_2,uVar2);
  FUN_004a2b20(param_1,&DAT_00570398,local_18,local_14);
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

