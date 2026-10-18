/* Entry: 0x004ec290; symbol: FUN_004ec290; body bytes: 228 */

void __cdecl
FUN_004ec290(undefined4 *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
            undefined4 param_5,undefined4 param_6)

{
  undefined4 *puVar1;
  int *unaff_FS_OFFSET;
  undefined4 uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined1 *puVar6;
  undefined1 local_14c [4];
  undefined4 local_148 [69];
  char *local_34 [8];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056a230;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_0040c080(local_148,0x114);
  uVar5 = 0x100;
  puVar1 = (undefined4 *)move<>(&param_2);
  FUN_004f3eb0(local_148,*puVar1,uVar5);
  local_8 = 0;
  puVar6 = &stack0x0000001c;
  uVar5 = param_3;
  uVar2 = param_4;
  uVar3 = param_5;
  uVar4 = param_6;
  puVar1 = FUN_0040af10(local_14c,local_148);
  FUN_004f3fb0(local_34,*puVar1,uVar5,uVar2,uVar3,uVar4,puVar6);
  FUN_004ee130(param_3,param_4,local_34);
  FUN_004fcc30(local_148,param_1);
  local_8 = 0xffffffff;
  FUN_004f51c0(local_148);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

