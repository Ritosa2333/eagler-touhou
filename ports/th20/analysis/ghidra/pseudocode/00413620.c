/* Entry: 0x00413620; symbol: FUN_00413620; body bytes: 94 */

void __cdecl FUN_00413620(void *param_1,char **param_2)

{
  char **ppcVar1;
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_005676c0;
  local_10 = *unaff_FS_OFFSET;
  ppcVar1 = (char **)(DAT_005b25c0 ^ (uint)&stack0xfffffffc);
  *unaff_FS_OFFSET = (int)&local_10;
  std::_Adl_verify_range<char*,char_const*>(param_2,ppcVar1);
  std::_Adl_verify_range<char*,char_const*>(param_2 + 1,ppcVar1);
  FUN_00415440(param_1,param_2,1);
  *unaff_FS_OFFSET = local_10;
  return;
}

