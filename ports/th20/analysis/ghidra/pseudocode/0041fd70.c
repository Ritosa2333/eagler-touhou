/* Entry: 0x0041fd70; symbol: FUN_0041fd70; body bytes: 120 */

void __fastcall FUN_0041fd70(CDocObjectServer *param_1)

{
  char **ppcVar1;
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567600;
  local_10 = *unaff_FS_OFFSET;
  ppcVar1 = (char **)(DAT_005b25c0 ^ (uint)&stack0xfffffffc);
  *unaff_FS_OFFSET = (int)&local_10;
  *(undefined ***)param_1 = &PTR_FUN_0056d4d0;
  std::_Adl_verify_range<char*,char_const*>((char **)"shutdown InputInf\n",ppcVar1);
  FUN_004124b0(DAT_005b66d8,*(void **)(param_1 + 8));
  FUN_004202c0(param_1);
  DAT_005b8898 = 0;
  FUN_0041fdf0((undefined4 *)param_1);
  *unaff_FS_OFFSET = local_10;
  return;
}

