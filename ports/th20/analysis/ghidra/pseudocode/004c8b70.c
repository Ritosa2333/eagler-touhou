/* Entry: 0x004c8b70; symbol: FUN_004c8b70; body bytes: 129 */

void __fastcall FUN_004c8b70(undefined4 *param_1)

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
  *param_1 = &PTR_FUN_00571400;
  std::_Adl_verify_range<char*,char_const*>((char **)"shutdown LaserInf\n",ppcVar1);
  FUN_004124b0(DAT_005b66d8,(void *)param_1[2]);
  FUN_004124b0(DAT_005b66d8,(void *)param_1[3]);
  FUN_004bc480((int)param_1);
  FUN_0041fdf0(param_1);
  *unaff_FS_OFFSET = local_10;
  return;
}

