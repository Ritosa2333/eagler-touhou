/* Entry: 0x00410840; symbol: FUN_00410840; body bytes: 323 */

void __cdecl FUN_00410840(char **param_1)

{
  undefined4 uVar1;
  LPCWSTR lpFileName;
  int *unaff_FS_OFFSET;
  DWORD DVar2;
  DWORD DVar3;
  LPSECURITY_ATTRIBUTES lpSecurityAttributes;
  DWORD DVar4;
  HLOCAL *lpBuffer;
  DWORD dwFlagsAndAttributes;
  HANDLE hTemplateFile;
  va_list *Arguments;
  HLOCAL local_34;
  function<void___cdecl(void)> local_30 [24];
  undefined4 local_18;
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  int local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00567a55;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  uVar1 = FUN_0040c310(&DAT_005c0240,2);
  FUN_0040b730(&local_18,uVar1);
  local_8 = 0;
  FUN_0040c720(local_30,&param_1);
  local_8._0_1_ = 1;
  hTemplateFile = (HANDLE)0x0;
  dwFlagsAndAttributes = 0x80;
  DVar4 = 2;
  lpSecurityAttributes = (LPSECURITY_ATTRIBUTES)0x0;
  DVar3 = 1;
  DVar2 = 0x40000000;
  lpFileName = (LPCWSTR)FUN_0040ff70((undefined4 *)local_30);
  DAT_005ae060 = CreateFileW(lpFileName,DVar2,DVar3,lpSecurityAttributes,DVar4,dwFlagsAndAttributes,
                             hTemplateFile);
  if (DAT_005ae060 == (HANDLE)0xffffffff) {
    local_34 = (HLOCAL)0x0;
    Arguments = (va_list *)0x0;
    DVar4 = 0;
    lpBuffer = &local_34;
    DVar3 = 0x400;
    DVar2 = GetLastError();
    FormatMessageW(0x1300,(LPCVOID)0x0,DVar2,DVar3,(LPWSTR)lpBuffer,DVar4,Arguments);
    std::_Adl_verify_range<char*,char_const*>((char **)"error : %s write error %s\r\n",param_1);
    LocalFree(local_34);
    local_8 = (uint)local_8._1_3_ << 8;
    FUN_0040e6c0(local_30);
    local_8 = 0xffffffff;
    FUN_0040b900(&local_18);
  }
  else {
    std::_Adl_verify_range<char*,char_const*>((char **)"%s open ...\r\n",param_1);
    local_8 = (uint)local_8._1_3_ << 8;
    FUN_0040e6c0(local_30);
    local_8 = 0xffffffff;
    FUN_0040b900(&local_18);
  }
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

