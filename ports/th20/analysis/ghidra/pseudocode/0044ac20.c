/* Entry: 0x0044ac20; symbol: FUN_0044ac20; body bytes: 417 */

void __cdecl
FUN_0044ac20(void *param_1,undefined8 *param_2,int param_3,undefined4 param_4,undefined4 param_5,
            int param_6,int param_7,int param_8)

{
  uint uVar1;
  undefined4 uVar2;
  int iVar3;
  int *unaff_FS_OFFSET;
  wchar_t *in_stack_00000050;
  undefined1 auStack_568 [24];
  undefined4 uStack_550;
  int local_528;
  undefined4 local_524;
  undefined4 local_520;
  undefined4 local_51c;
  undefined4 local_518;
  wchar_t local_514 [640];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  int local_8;
  
  puStack_c = &LAB_00568528;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  local_8 = 0;
  uVar2 = FUN_0040c310(&DAT_005c0240,9);
  FUN_0040b730(&local_518,uVar2);
  local_8._0_1_ = 1;
  uStack_550 = 0x44ac8e;
  FUN_00447940(local_514,in_stack_00000050,&stack0x00000054);
  iVar3 = FUN_00437cd0(param_3);
  local_528 = *(int *)(param_3 + 0x3a8);
  local_524 = *(undefined4 *)(param_3 + 0x3ac);
  local_520 = *(undefined4 *)(param_3 + 0x3b0);
  local_51c = *(undefined4 *)(param_3 + 0x3b4);
  *(byte *)(param_3 + 0x49a) = *(byte *)(param_3 + 0x49a) | 1;
  uVar1 = *(uint *)(param_3 + 0x49c);
  uVar2 = FUN_00445a00(param_1,*(uint *)(iVar3 + 8));
  function<>(auStack_568,(int)&stack0x00000028);
  FUN_0044fff0(param_2,uVar2,iVar3,param_7 << 1,param_4,param_5,(LPCSTR)local_514,uVar1 >> 10 & 1,
               param_6,param_8 << 1,&local_528);
  local_8 = (uint)local_8._1_3_ << 8;
  FUN_0040b900(&local_518);
  local_8 = 0xffffffff;
  FUN_00414400(&stack0x00000028);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

