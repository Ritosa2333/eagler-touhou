/* Entry: 0x00564f1e; symbol: FUN_00564f1e; body bytes: 540 */

int __cdecl FUN_00564f1e(char *param_1,uint *param_2,uint param_3,int *param_4)

{
  bool bVar1;
  DWORD *pDVar2;
  BOOL BVar3;
  DWORD DVar4;
  DWORD local_34 [5];
  DWORD local_20;
  int local_1c;
  int local_18;
  char *local_14;
  uint local_10;
  uint *local_c;
  char local_5;
  
  local_14 = param_1;
  local_c = param_2;
  local_10 = param_3;
  if (param_3 == 0) {
    return 0;
  }
  if (param_2 == (uint *)0x0) {
LAB_00564f4a:
    *(undefined1 *)(param_4 + 9) = 1;
    param_4[8] = 0;
    *(undefined1 *)(param_4 + 7) = 1;
    param_4[6] = 0x16;
    FUN_0054803f((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0,param_4);
    return -1;
  }
  local_18 = (int)param_1 >> 6;
  local_1c = ((uint)param_1 & 0x3f) * 0x38;
  local_5 = *(char *)(local_1c + 0x29 + (&DAT_005e5690)[local_18]);
  if (((local_5 == '\x02') || (local_5 == '\x01')) && ((~param_3 & 1) == 0)) goto LAB_00564f4a;
  if ((*(byte *)(local_1c + 0x28 + (&DAT_005e5690)[local_18]) & 0x20) != 0) {
    FUN_005651bd((uint)param_1,0,0,(PLARGE_INTEGER)0x2,(int)param_4);
  }
  DVar4 = 0;
  local_34[3] = 0;
  bVar1 = FUN_00564a9a((uint)local_14,param_4);
  if (bVar1) {
    if (local_5 == '\0') {
      pDVar2 = (DWORD *)FUN_00564667(local_34,(uint)local_14,local_c,local_10,param_4);
    }
    else {
      if ((local_5 != '\x01') && (local_5 != '\x02')) goto LAB_005650d1;
      pDVar2 = (DWORD *)write_double_translated_unicode_nolock((char *)local_34,(uint)local_c);
    }
  }
  else if (*(char *)((&DAT_005e5690)[local_18] + 0x28 + local_1c) < '\0') {
    if (local_5 == '\0') {
      pDVar2 = (DWORD *)write_text_ansi_nolock((int)local_34,local_14,(uint)local_c);
    }
    else if (local_5 == '\x01') {
      pDVar2 = (DWORD *)write_text_utf8_nolock((int)local_34,local_14,(uint)local_c);
    }
    else {
      if (local_5 != '\x02') goto LAB_005650d1;
      pDVar2 = (DWORD *)write_text_utf16le_nolock((int)local_34,local_14,(uint)local_c);
    }
  }
  else {
    local_34[0] = 0;
    local_34[1] = 0;
    local_34[2] = 0;
    BVar3 = WriteFile(*(HANDLE *)((&DAT_005e5690)[local_18] + 0x18 + local_1c),local_c,local_10,
                      local_34 + 1,(LPOVERLAPPED)0x0);
    if (BVar3 == 0) {
      local_34[0] = GetLastError();
    }
    pDVar2 = local_34;
  }
  DVar4 = *pDVar2;
  local_34[3] = DVar4;
  local_34[4] = pDVar2[1];
  local_20 = pDVar2[2];
  if (local_34[4] != 0) {
    return local_34[4] - local_20;
  }
LAB_005650d1:
  if (DVar4 != 0) {
    if (DVar4 == 5) {
      *(undefined1 *)(param_4 + 7) = 1;
      param_4[6] = 9;
      *(undefined1 *)(param_4 + 9) = 1;
      param_4[8] = 5;
      return -1;
    }
    FUN_00551688(DVar4,(int)param_4);
    return -1;
  }
  if (((*(byte *)((&DAT_005e5690)[local_18] + 0x28 + local_1c) & 0x40) != 0) &&
     ((char)*local_c == '\x1a')) {
    return 0;
  }
  *(undefined1 *)(param_4 + 7) = 1;
  param_4[6] = 0x1c;
  *(undefined1 *)(param_4 + 9) = 1;
  param_4[8] = 0;
  return -1;
}

