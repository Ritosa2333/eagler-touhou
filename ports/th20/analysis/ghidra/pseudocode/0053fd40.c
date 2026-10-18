/* Entry: 0x0053fd40; symbol: FUN_0053fd40; body bytes: 99 */

void __fastcall FUN_0053fd40(undefined4 *param_1)

{
  int iVar1;
  char **ppcVar2;
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567750;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  iVar1 = FUN_004fbff0((int)param_1);
  ppcVar2 = (char **)move<>(iVar1);
  FUN_004ae9d0(*param_1,ppcVar2);
  *unaff_FS_OFFSET = local_10;
  return;
}

