/* Entry: 0x00411060; symbol: FUN_00411060; body bytes: 255 */

void __cdecl FUN_00411060(LPCVOID param_1,DWORD param_2)

{
  char **ppcVar1;
  undefined4 uVar2;
  int *unaff_FS_OFFSET;
  DWORD local_1c;
  undefined4 local_18;
  char **local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_005676ed;
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
    WriteFile(DAT_005ae060,param_1,param_2,&local_1c,(LPOVERLAPPED)0x0);
    if (param_2 == local_1c) {
      std::_Adl_verify_range<char*,char_const*>((char **)"write ...\r\n",ppcVar1);
      local_8 = 0xffffffff;
      FUN_0040b900(&local_18);
    }
    else {
      CloseHandle(DAT_005ae060);
      std::_Adl_verify_range<char*,char_const*>((char **)"error : write error\r\n",ppcVar1);
      local_8 = 0xffffffff;
      FUN_0040b900(&local_18);
    }
  }
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee((uint)local_14 ^ (uint)&stack0xfffffffc);
  return;
}

