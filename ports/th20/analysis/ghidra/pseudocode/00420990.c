/* Entry: 0x00420990; symbol: FUN_00420990; body bytes: 265 */

undefined4 __fastcall FUN_00420990(CDocObjectServer *param_1)

{
  CDocObjectServer *pCVar1;
  int iVar2;
  char **in_stack_fffffff0;
  int local_8;
  
  std::_Adl_verify_range<char*,char_const*>((char **)"start InputInf\n",in_stack_fffffff0);
  *(undefined4 *)(param_1 + 0x10) = 0;
  for (local_8 = 0; local_8 < 2; local_8 = local_8 + 1) {
    pCVar1 = param_1 + local_8 * 0x30 + 0x2e38;
    *(undefined4 *)pCVar1 = (&DAT_005c4f10)[local_8 * 0xc];
    *(undefined4 *)(pCVar1 + 4) = (&DAT_005c4f14)[local_8 * 0xc];
    *(undefined4 *)(pCVar1 + 8) = *(undefined4 *)(&DAT_005c4f18 + local_8 * 0x30);
    *(undefined4 *)(pCVar1 + 0xc) = *(undefined4 *)(&DAT_005c4f1c + local_8 * 0x30);
    iVar2 = local_8 * 0x30;
    pCVar1 = param_1 + local_8 * 0x30 + 0x2e48;
    *(undefined4 *)pCVar1 = *(undefined4 *)(&DAT_005c4f20 + iVar2);
    *(undefined4 *)(pCVar1 + 4) = *(undefined4 *)(&DAT_005c4f24 + iVar2);
    *(undefined4 *)(pCVar1 + 8) = *(undefined4 *)(&DAT_005c4f28 + iVar2);
    *(undefined4 *)(pCVar1 + 0xc) = *(undefined4 *)(&DAT_005c4f2c + iVar2);
    iVar2 = local_8 * 0x30;
    pCVar1 = param_1 + local_8 * 0x30 + 0x2e58;
    *(undefined4 *)pCVar1 = *(undefined4 *)(&DAT_005c4f30 + iVar2);
    *(undefined4 *)(pCVar1 + 4) = *(undefined4 *)(&DAT_005c4f34 + iVar2);
    *(undefined4 *)(pCVar1 + 8) = *(undefined4 *)(&DAT_005c4f38 + iVar2);
    *(undefined4 *)(pCVar1 + 0xc) = *(undefined4 *)(&DAT_005c4f3c + iVar2);
  }
  FUN_00420d80(param_1);
  for (iVar2 = 0; iVar2 < 4; iVar2 = iVar2 + 1) {
    (&DAT_005b889c)[iVar2] = &DAT_005b88b0 + iVar2 * 0xb0;
  }
  return 0;
}

