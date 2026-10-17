/* Entry: 0x00410cf0; symbol: FUN_00410cf0; body bytes: 368 */

void __cdecl FUN_00410cf0(char **param_1)

{
  undefined4 uVar1;
  DWORD dwMessageId;
  int *unaff_FS_OFFSET;
  DWORD dwLanguageId;
  HLOCAL *lpBuffer;
  DWORD nSize;
  va_list *Arguments;
  HLOCAL local_228;
  undefined4 local_224;
  WCHAR local_220 [262];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00567ad0;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  uVar1 = FUN_0040c310(&DAT_005c0240,2);
  FUN_0040b730(&local_224,uVar1);
  local_8 = 0;
  _memset(local_220,0,0x20a);
  MultiByteToWideChar(0x3a4,0,(LPCSTR)param_1,-1,local_220,0x104);
  DAT_005ae060 = CreateFileW(local_220,0x80000000,1,(LPSECURITY_ATTRIBUTES)0x0,3,0x8000080,
                             (HANDLE)0x0);
  if (DAT_005ae060 == (HANDLE)0xffffffff) {
    local_228 = (HLOCAL)0x0;
    Arguments = (va_list *)0x0;
    nSize = 0;
    lpBuffer = &local_228;
    dwLanguageId = 0x400;
    dwMessageId = GetLastError();
    FormatMessageW(0x1300,(LPCVOID)0x0,dwMessageId,dwLanguageId,(LPWSTR)lpBuffer,nSize,Arguments);
    std::_Adl_verify_range<char*,char_const*>((char **)"error : %s write error %s\r\n",param_1);
    LocalFree(local_228);
    local_8 = 0xffffffff;
    FUN_0040b900(&local_224);
  }
  else {
    std::_Adl_verify_range<char*,char_const*>((char **)"%s open ...\r\n",param_1);
    local_8 = 0xffffffff;
    FUN_0040b900(&local_224);
  }
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

