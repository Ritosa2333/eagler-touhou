/* Entry: 0x004106e0; symbol: FUN_004106e0; body bytes: 179 */

void FUN_004106e0(void)

{
  char **ppcVar1;
  undefined4 uVar2;
  int *unaff_FS_OFFSET;
  undefined4 local_18;
  char **local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_005679cd;
  local_10 = *unaff_FS_OFFSET;
  ppcVar1 = (char **)(DAT_005b25c0 ^ (uint)&stack0xfffffffc);
  *unaff_FS_OFFSET = (int)&local_10;
  local_14 = ppcVar1;
  uVar2 = FUN_0040c310(&DAT_005c0240,2);
  FUN_0040b730(&local_18,uVar2);
  local_8 = 0;
  if (DAT_005ae060 == (HANDLE)0xffffffff) {
    local_8 = 0xffffffff;
    FUN_0040b900(&local_18);
  }
  else {
    CloseHandle(DAT_005ae060);
    std::_Adl_verify_range<char*,char_const*>((char **)"close ...\r\n",ppcVar1);
    local_8 = 0xffffffff;
    FUN_0040b900(&local_18);
  }
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee((uint)local_14 ^ (uint)&stack0xfffffffc);
  return;
}

