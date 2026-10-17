/* Entry: 0x004d9210; symbol: FUN_004d9210; body bytes: 521 */

void FUN_004d9210(void)

{
  int iVar1;
  LPVOID pvVar2;
  BITMAPINFO *lpbmi;
  HDC hdc;
  HBITMAP ho;
  int iVar3;
  int local_30;
  int local_28;
  undefined1 *local_24;
  undefined1 *local_1c;
  undefined2 local_18;
  undefined4 uStack_16;
  undefined2 uStack_12;
  undefined2 local_10;
  undefined2 uStack_e;
  undefined2 local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  lpbmi = (BITMAPINFO *)FUN_004d8740();
  local_18 = 0;
  uStack_16._0_2_ = 0;
  uStack_16._2_2_ = 0;
  uStack_12 = 0;
  local_10 = 0;
  uStack_e = 0;
  local_c = 0;
  _memset(&local_18,0,0xe);
  local_18 = 0x4d42;
  uStack_e = 0x36;
  local_c = 0;
  uStack_16 = DAT_005b87f8 * 3 * DAT_005b87fc + 0x36;
  (lpbmi->bmiHeader).biBitCount = 0x18;
  (lpbmi->bmiHeader).biSize = 0x28;
  (lpbmi->bmiHeader).biWidth = DAT_005b87f8;
  (lpbmi->bmiHeader).biHeight = DAT_005b87fc;
  (lpbmi->bmiHeader).biPlanes = 1;
  (lpbmi->bmiHeader).biCompression = 0;
  pvVar2 = DAT_005c59c4;
  hdc = GetDC((HWND)0x0);
  ho = CreateDIBSection(hdc,lpbmi,0,&local_1c,(HANDLE)0x0,0);
  iVar1 = DAT_005c538c * 4;
  iVar3 = DAT_005c5390 * DAT_005c59c8;
  local_28 = DAT_005b87fc;
  while (local_28 = local_28 + -1, -1 < local_28) {
    local_24 = (undefined1 *)((int)pvVar2 + DAT_005c59c8 * local_28 + iVar3 + iVar1);
    for (local_30 = 0; local_30 < DAT_005b87f8; local_30 = local_30 + 1) {
      local_1c[2] = local_24[2];
      local_1c[1] = local_24[1];
      *local_1c = *local_24;
      local_24 = local_24 + 4;
      local_1c = local_1c + 3;
    }
  }
  FUN_004d9c40(ho,&DAT_005c59cc);
  DeleteObject(ho);
  if (lpbmi != (BITMAPINFO *)0x0) {
    FUN_0041f670(lpbmi);
  }
  if (DAT_005c59c4 != (LPVOID)0x0) {
    FUN_0041f670(DAT_005c59c4);
    DAT_005c59c4 = (LPVOID)0x0;
  }
  ReleaseDC((HWND)0x0,hdc);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

