/* Entry: 0x00489ac0; symbol: FUN_00489ac0; body bytes: 104 */

void __cdecl FUN_00489ac0(void *param_1,char **param_2)

{
  char **ppcVar1;
  void *pvVar2;
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
  pvVar2 = (void *)move<>(param_2 + 1);
  FUN_0048b070(param_1,pvVar2);
  FUN_00498020(param_1,param_2,1);
  *unaff_FS_OFFSET = local_10;
  return;
}

