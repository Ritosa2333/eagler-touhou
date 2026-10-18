/* Entry: 0x00541217; symbol: FUN_00541217; body bytes: 650 */

void FUN_00541217(LPCWSTR param_1,DWORD *param_2,uint param_3,uint param_4)

{
  LPCWSTR hFile;
  BOOL BVar1;
  DWORD DVar2;
  HANDLE hFindFile;
  uint uVar3;
  uint uVar4;
  undefined1 local_2bc [4];
  LPCWSTR local_2b8;
  DWORD local_2b4;
  DWORD local_2b0;
  DWORD local_2ac;
  byte local_2a5;
  _WIN32_FIND_DATAW local_2a4;
  undefined1 local_54 [16];
  DWORD local_44;
  DWORD local_40;
  uint local_34;
  uint local_2c [3];
  undefined1 local_20 [8];
  DWORD local_18;
  DWORD local_14;
  DWORD local_10;
  DWORD local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_2a5 = (byte)param_3 & 1;
  uVar3 = param_3 & 0xfffffffe;
  local_2b8 = param_1;
  if (((param_3 & 1) != 0) && ((param_3 >> 2 & 1) != 0)) goto LAB_00541490;
  if ((((param_3 >> 1 & 1) != 0) && (param_4 != 0xffffffff)) &&
     (((param_4 >> 10 & 1) == 0 || ((param_3 & 1) == 0)))) {
    uVar3 = param_3 & 0xfffffffc;
    param_2[4] = param_4;
  }
  if (uVar3 == 0) goto LAB_00541490;
  uVar4 = uVar3;
  if (((uVar3 & 0x2a) != 0) &&
     (((param_4 == 0xffffffff || ((param_4 >> 10 & 1) == 0)) || ((param_3 & 1) == 0)))) {
    BVar1 = GetFileAttributesExW(param_1,GetFileExInfoStandard,local_2c);
    if (BVar1 == 0) {
      DVar2 = GetLastError();
      if (DVar2 != 0x20) goto LAB_00541490;
      hFindFile = FindFirstFileW(param_1,&local_2a4);
      if (hFindFile == (HANDLE)0xffffffff) {
        GetLastError();
        goto LAB_00541490;
      }
      FindClose(hFindFile);
      local_2b0 = local_2a4.nFileSizeLow;
      local_2b4 = local_2a4.ftLastWriteTime.dwLowDateTime;
      local_2ac = local_2a4.ftLastWriteTime.dwHighDateTime;
    }
    else {
      local_2b0 = local_c;
      local_2ac = local_14;
      local_2b4 = local_18;
      local_2a4.dwFileAttributes = local_2c[0];
      local_2a4.nFileSizeHigh = local_10;
    }
    if ((local_2a5 == 0) || ((local_2a4.dwFileAttributes >> 10 & 1) == 0)) {
      param_2[4] = local_2a4.dwFileAttributes;
      uVar4 = uVar3 & 0xffffffd5;
      param_2[3] = local_2a4.nFileSizeHigh;
      param_2[2] = local_2b0;
      *param_2 = local_2b4;
      param_2[1] = local_2ac;
      if (((local_2a4.dwFileAttributes >> 10 & 1) == 0) && ((uVar4 >> 2 & 1) != 0)) {
        param_2[5] = 0;
        uVar4 = uVar3 & 0xffffffd1;
      }
    }
    if (uVar4 == 0) goto LAB_00541490;
  }
  local_2ac = ___std_fs_open_handle_16
                        (&local_2b8,local_2b8,0x80,((local_2a5 ^ 1) + 0x10) * 0x200000);
  hFile = local_2b8;
  if (local_2ac == 0) {
    uVar3 = uVar4;
    if ((uVar4 & 0x26) == 0) {
LAB_0054144c:
      if ((uVar3 & 0x18) != 0) {
        BVar1 = GetFileInformationByHandleEx(hFile,FileStandardInfo,local_20,0x18);
        if (BVar1 == 0) goto LAB_005413ef;
        param_2[2] = local_18;
        param_2[3] = local_14;
        param_2[6] = local_10;
      }
    }
    else {
      BVar1 = GetFileInformationByHandleEx(local_2b8,FileBasicInfo,local_54,0x28);
      if (BVar1 != 0) {
        uVar3 = uVar4 & 0xffffffdd;
        *param_2 = local_44;
        param_2[4] = local_34;
        param_2[1] = local_40;
        if ((uVar3 >> 2 & 1) != 0) {
          if ((local_34 & 0x400) == 0) {
            param_2[5] = 0;
          }
          else {
            BVar1 = GetFileInformationByHandleEx(hFile,FileAttributeTagInfo,local_2bc,8);
            if (BVar1 == 0) goto LAB_005413ef;
            param_2[5] = (DWORD)local_2b8;
          }
          uVar3 = uVar4 & 0xffffffd9;
        }
        goto LAB_0054144c;
      }
LAB_005413ef:
      GetLastError();
    }
  }
  FUN_00541026(hFile);
LAB_00541490:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

