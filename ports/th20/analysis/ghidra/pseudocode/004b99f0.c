/* Entry: 0x004b99f0; symbol: FUN_004b99f0; body bytes: 164 */

void FUN_004b99f0(undefined4 param_1)

{
  undefined4 uVar1;
  int *unaff_FS_OFFSET;
  undefined4 local_18;
  char **local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056808d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = (char **)(DAT_005b25c0 ^ (uint)&stack0xfffffffc);
  *unaff_FS_OFFSET = (int)&local_10;
  std::_Adl_verify_range<char*,char_const*>((char **)"StartSubThread\n",local_14);
  uVar1 = FUN_0040c310(&DAT_005c0240,6);
  FUN_0040b730(&local_18,uVar1);
  local_8 = 0;
  FUN_0040b1d0(param_1,&stack0x00000008);
  local_8 = 0xffffffff;
  FUN_0040b900(&local_18);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee((uint)local_14 ^ (uint)&stack0xfffffffc);
  return;
}

