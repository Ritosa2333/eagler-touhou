/* Entry: 0x0052c030; symbol: FUN_0052c030; body bytes: 344 */

void __fastcall FUN_0052c030(int param_1)

{
  undefined4 uVar1;
  int *piVar2;
  int *unaff_FS_OFFSET;
  undefined4 local_18;
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056808d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  uVar1 = FUN_0040c310(&DAT_005c0240,9);
  FUN_0040b730(&local_18,uVar1);
  local_8 = 0;
  FUN_004a2800(*(void **)(param_1 + 0x5924));
  *(undefined4 *)(param_1 + 0x5924) = 0;
  *(undefined4 *)(param_1 + 0x5924) = 0;
  piVar2 = FUN_004712d0();
  *(int **)(param_1 + 0x5924) = piVar2;
  FUN_0049d5b0(*(void **)(param_1 + 0x5924),0,0.0,(float)DAT_005b87f8,(float)DAT_005b87fc);
  FUN_00452f20((void *)(param_1 + 0x5928),0.0);
  FUN_0052cd50(*(void **)(param_1 + 0x5924),FUN_0051d170);
  FUN_0051e030((void *)(param_1 + 0x5930),0xffd0d0d0);
  *(undefined4 *)(param_1 + 0x592c) = DAT_005756a0;
  local_8 = 0xffffffff;
  FUN_0040b900(&local_18);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

