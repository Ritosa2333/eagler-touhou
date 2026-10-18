/* Entry: 0x00410aa0; symbol: FUN_00410aa0; body bytes: 583 */

void __cdecl FUN_00410aa0(undefined1 (*param_1) [16],SIZE_T *param_2,int param_3)

{
  undefined4 uVar1;
  undefined1 (*pauVar2) [16];
  uint *puVar3;
  LPCWSTR lpFileName;
  LPVOID lpBuffer;
  int *unaff_FS_OFFSET;
  DWORD dwDesiredAccess;
  DWORD dwShareMode;
  LPSECURITY_ATTRIBUTES lpSecurityAttributes;
  DWORD dwCreationDisposition;
  DWORD dwFlagsAndAttributes;
  HANDLE pvVar4;
  undefined1 (*local_38) [16];
  SIZE_T local_34;
  function<void___cdecl(void)> local_30 [24];
  undefined4 local_18;
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00567a8d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  uVar1 = FUN_0040c310(&DAT_005c0240,2);
  FUN_0040b730(&local_18,uVar1);
  local_8 = 0;
  if (param_3 == 0) {
    pauVar2 = FUN_005448c0(param_1,0x5c);
    if (pauVar2 == (undefined1 (*) [16])0x0) {
      local_38 = param_1;
    }
    else {
      local_38 = (undefined1 (*) [16])(*pauVar2 + 1);
    }
    pauVar2 = FUN_005448c0(local_38,0x2f);
    if (pauVar2 == (undefined1 (*) [16])0x0) {
      local_38 = param_1;
    }
    else {
      local_38 = (undefined1 (*) [16])(*pauVar2 + 1);
    }
    local_34 = FUN_0053a620(&DAT_005b66c8,(byte *)local_38);
    if (param_2 != (SIZE_T *)0x0) {
      *param_2 = local_34;
    }
    if (local_34 != 0) {
      if (local_34 == 0) goto LAB_00410bb8;
      std::_Adl_verify_range<char*,char_const*>((char **)"%s Decode ... \r\n",(char **)local_38);
      puVar3 = (uint *)FUN_0041f610(local_34);
      if (puVar3 == (uint *)0x0) goto LAB_00410cb5;
      FUN_0053a3c0(&DAT_005b66c8,(byte *)local_38,puVar3);
LAB_00410c9b:
      local_8 = 0xffffffff;
      FUN_0040b900(&local_18);
      goto LAB_00410cce;
    }
  }
  else {
LAB_00410bb8:
    std::_Adl_verify_range<char*,char_const*>((char **)"%s Load ... \r\n",(char **)param_1);
    FUN_0040c720(local_30,&param_1);
    pvVar4 = (HANDLE)0x0;
    dwFlagsAndAttributes = 0x8000080;
    dwCreationDisposition = 3;
    lpSecurityAttributes = (LPSECURITY_ATTRIBUTES)0x0;
    dwShareMode = 1;
    dwDesiredAccess = 0x80000000;
    lpFileName = (LPCWSTR)FUN_0040ff70((undefined4 *)local_30);
    pvVar4 = CreateFileW(lpFileName,dwDesiredAccess,dwShareMode,lpSecurityAttributes,
                         dwCreationDisposition,dwFlagsAndAttributes,pvVar4);
    FUN_0040e6c0(local_30);
    if (pvVar4 == (HANDLE)0xffffffff) {
      std::_Adl_verify_range<char*,char_const*>
                ((char **)"error : %s is not found.\r\n",(char **)param_1);
    }
    else {
      local_34 = GetFileSize(pvVar4,(LPDWORD)0x0);
      lpBuffer = (LPVOID)FUN_0041f610(local_34);
      if (lpBuffer != (LPVOID)0x0) {
        ReadFile(pvVar4,lpBuffer,local_34,&local_34,(LPOVERLAPPED)0x0);
        if (param_2 != (SIZE_T *)0x0) {
          *param_2 = local_34;
        }
        CloseHandle(pvVar4);
        goto LAB_00410c9b;
      }
      std::_Adl_verify_range<char*,char_const*>
                ((char **)"error : %s allocation error.\r\n",(char **)param_1);
      CloseHandle(pvVar4);
    }
  }
LAB_00410cb5:
  local_8 = 0xffffffff;
  FUN_0040b900(&local_18);
LAB_00410cce:
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

