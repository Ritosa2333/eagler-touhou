/* Entry: 0x004a0990; symbol: FUN_004a0990; body bytes: 110 */

undefined4 __fastcall FUN_004a0990(int param_1)

{
  int iVar1;
  int *piVar2;
  
  iVar1 = FUN_0044eb30(DAT_005c0028,*(int *)(param_1 + 0xec) + 0xf,*(char ***)(param_1 + 0x78));
  piVar2 = (int *)FUN_00414580((void *)(param_1 + 0x8c),*(int *)(param_1 + 0xec));
  *piVar2 = iVar1;
  *(uint *)(param_1 + 0x7c) = *(uint *)(param_1 + 0x7c) & 0xfffffffb;
  FUN_004a05d0(DAT_005c0698);
  return 0;
}

