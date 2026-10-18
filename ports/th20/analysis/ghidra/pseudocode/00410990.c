/* Entry: 0x00410990; symbol: FUN_00410990; body bytes: 266 */

void __cdecl FUN_00410990(SIZE_T param_1)

{
  char **ppcVar1;
  undefined4 uVar2;
  LPVOID lpBuffer;
  int *unaff_FS_OFFSET;
  DWORD local_1c;
  undefined4 local_18;
  char **local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056755d;
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
    lpBuffer = (LPVOID)FUN_0041f610(param_1);
    if (lpBuffer == (LPVOID)0x0) {
      CloseHandle(DAT_005ae060);
      local_8 = 0xffffffff;
      FUN_0040b900(&local_18);
    }
    else {
      ReadFile(DAT_005ae060,lpBuffer,param_1,&local_1c,(LPOVERLAPPED)0x0);
      std::_Adl_verify_range<char*,char_const*>((char **)"Read ...\r\n",ppcVar1);
      local_8 = 0xffffffff;
      FUN_0040b900(&local_18);
    }
  }
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee((uint)local_14 ^ (uint)&stack0xfffffffc);
  return;
}

