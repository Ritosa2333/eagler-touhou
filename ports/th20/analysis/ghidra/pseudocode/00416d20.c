/* Entry: 0x00416d20; symbol: FUN_00416d20; body bytes: 3437 */

void FUN_00416d20(void)

{
  HDC hdc;
  HFONT pHVar1;
  undefined4 *puVar2;
  LPCWSTR pWStack_6c;
  wchar_t *pwStack_68;
  tagLOGFONTW local_64;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  hdc = GetDC((HWND)0x0);
  _memset(&local_64,0,0x5c);
  DAT_005b6748 = &DAT_005b66ec;
  FUN_00548100(local_64.lfFaceName,0x20,0x56c820);
  EnumFontFamiliesExW(hdc,&local_64,FUN_00414820,0,0);
  DAT_005b6748 = &DAT_005b66ee;
  FUN_00548100(local_64.lfFaceName,0x20,0x56c82c);
  EnumFontFamiliesExW(hdc,&local_64,FUN_00414820,0,0);
  if (DAT_005b66ed == '\0') {
    if (DAT_005b66ec == '\0') {
      pWStack_6c = L"ＭＳ ゴシック";
      pHVar1 = CreateFontW(0x18,0,0,0,400,0,0,0,0x80,0,0,2,0x11,L"ＭＳ ゴシック");
      puVar2 = (undefined4 *)FUN_00414580(&DAT_005b66f0,2);
      *puVar2 = pHVar1;
      pHVar1 = CreateFontW(0x1c,0,0,0,400,0,0,0,0x80,0,0,2,0x11,L"ＭＳ ゴシック");
      puVar2 = (undefined4 *)FUN_00414580(&DAT_005b66f0,3);
      *puVar2 = pHVar1;
      pHVar1 = CreateFontW(0x20,0,0,0,400,0,0,0,0x80,0,0,2,0x11,L"ＭＳ ゴシック");
      puVar2 = (undefined4 *)FUN_00414580(&DAT_005b66f0,4);
      *puVar2 = pHVar1;
      pHVar1 = CreateFontW(0x24,0,0,0,400,0,0,0,0x80,0,0,2,0x11,L"ＭＳ ゴシック");
      puVar2 = (undefined4 *)FUN_00414580(&DAT_005b66f0,5);
      *puVar2 = pHVar1;
      pHVar1 = CreateFontW(0x28,0,0,0,400,0,0,0,0x80,0,0,2,0x11,L"ＭＳ ゴシック");
      puVar2 = (undefined4 *)FUN_00414580(&DAT_005b66f0,6);
      *puVar2 = pHVar1;
      pHVar1 = CreateFontW(0x2c,0,0,0,400,0,0,0,0x80,0,0,2,0x11,L"ＭＳ ゴシック");
      puVar2 = (undefined4 *)FUN_00414580(&DAT_005b66f0,7);
      *puVar2 = pHVar1;
      pHVar1 = CreateFontW(0x30,0,0,0,400,0,0,0,0x80,0,0,2,0x11,L"ＭＳ ゴシック");
      puVar2 = (undefined4 *)FUN_00414580(&DAT_005b66f0,8);
      *puVar2 = pHVar1;
      pHVar1 = CreateFontW(0x3c,0,0,0,400,0,0,0,0x80,0,0,2,0x11,L"ＭＳ ゴシック");
      puVar2 = (undefined4 *)FUN_00414580(&DAT_005b66f0,9);
      *puVar2 = pHVar1;
      pHVar1 = CreateFontW(0x40,0,0,0,400,0,0,0,0x80,0,0,2,0x11,L"ＭＳ ゴシック");
      puVar2 = (undefined4 *)FUN_00414580(&DAT_005b66f0,10);
      *puVar2 = pHVar1;
      pHVar1 = CreateFontW(0x20,0,0,0,600,0,0,0,0x80,0,0,2,0x11,L"ＭＳ ゴシック");
      puVar2 = (undefined4 *)FUN_00414580(&DAT_005b66f0,0xb);
      *puVar2 = pHVar1;
    }
    else {
      pWStack_6c = L"メイリオ";
      pHVar1 = CreateFontW(0x24,0,0,0,400,0,0,0,0x80,0,0,2,0x11,L"メイリオ");
      puVar2 = (undefined4 *)FUN_00414580(&DAT_005b66f0,2);
      *puVar2 = pHVar1;
      pHVar1 = CreateFontW(0x2a,0,0,0,400,0,0,0,0x80,0,0,2,0x11,L"メイリオ");
      puVar2 = (undefined4 *)FUN_00414580(&DAT_005b66f0,3);
      *puVar2 = pHVar1;
      pHVar1 = CreateFontW(0x30,0,0,0,400,0,0,0,0x80,0,0,2,0x11,L"メイリオ");
      puVar2 = (undefined4 *)FUN_00414580(&DAT_005b66f0,4);
      *puVar2 = pHVar1;
      pHVar1 = CreateFontW(0x36,0,0,0,400,0,0,0,0x80,0,0,2,0x11,L"メイリオ");
      puVar2 = (undefined4 *)FUN_00414580(&DAT_005b66f0,5);
      *puVar2 = pHVar1;
      pHVar1 = CreateFontW(0x3c,0,0,0,400,0,0,0,0x80,0,0,2,0x11,L"メイリオ");
      puVar2 = (undefined4 *)FUN_00414580(&DAT_005b66f0,6);
      *puVar2 = pHVar1;
      pHVar1 = CreateFontW(0x42,0,0,0,400,0,0,0,0x80,0,0,2,0x11,L"メイリオ");
      puVar2 = (undefined4 *)FUN_00414580(&DAT_005b66f0,7);
      *puVar2 = pHVar1;
      pHVar1 = CreateFontW(0x48,0,0,0,400,0,0,0,0x80,0,0,2,0x11,L"メイリオ");
      puVar2 = (undefined4 *)FUN_00414580(&DAT_005b66f0,8);
      *puVar2 = pHVar1;
      pHVar1 = CreateFontW(0x5a,0,0,0,400,0,0,0,0x80,0,0,2,0x11,L"メイリオ");
      puVar2 = (undefined4 *)FUN_00414580(&DAT_005b66f0,9);
      *puVar2 = pHVar1;
      pHVar1 = CreateFontW(0x60,0,0,0,400,0,0,0,0x80,0,0,2,0x11,L"メイリオ");
      puVar2 = (undefined4 *)FUN_00414580(&DAT_005b66f0,10);
      *puVar2 = pHVar1;
      pHVar1 = CreateFontW(0x30,0,0,0,600,0,0,0,0x80,0,0,2,0x11,L"メイリオ");
      puVar2 = (undefined4 *)FUN_00414580(&DAT_005b66f0,0xb);
      *puVar2 = pHVar1;
    }
  }
  else {
    pWStack_6c = L"游ゴシック";
    pHVar1 = CreateFontW(0x18,0,0,0,400,0,0,0,0x80,0,0,2,0x11,L"游ゴシック");
    puVar2 = (undefined4 *)FUN_00414580(&DAT_005b66f0,0);
    *puVar2 = pHVar1;
    pHVar1 = CreateFontW(0x1e,0,0,0,400,0,0,0,0x80,0,0,2,0x11,L"游ゴシック");
    puVar2 = (undefined4 *)FUN_00414580(&DAT_005b66f0,1);
    *puVar2 = pHVar1;
    pHVar1 = CreateFontW(0x24,0,0,0,400,0,0,0,0x80,0,0,2,0x11,L"游ゴシック");
    puVar2 = (undefined4 *)FUN_00414580(&DAT_005b66f0,2);
    *puVar2 = pHVar1;
    pHVar1 = CreateFontW(0x2a,0,0,0,400,0,0,0,0x80,0,0,2,0x11,L"游ゴシック");
    puVar2 = (undefined4 *)FUN_00414580(&DAT_005b66f0,3);
    *puVar2 = pHVar1;
    pHVar1 = CreateFontW(0x30,0,0,0,400,0,0,0,0x80,0,0,2,0x11,L"游ゴシック");
    puVar2 = (undefined4 *)FUN_00414580(&DAT_005b66f0,4);
    *puVar2 = pHVar1;
    pHVar1 = CreateFontW(0x36,0,0,0,400,0,0,0,0x80,0,0,2,0x11,L"游ゴシック");
    puVar2 = (undefined4 *)FUN_00414580(&DAT_005b66f0,5);
    *puVar2 = pHVar1;
    pHVar1 = CreateFontW(0x3c,0,0,0,400,0,0,0,0x80,0,0,2,0x11,L"游ゴシック");
    puVar2 = (undefined4 *)FUN_00414580(&DAT_005b66f0,6);
    *puVar2 = pHVar1;
    pHVar1 = CreateFontW(0x42,0,0,0,400,0,0,0,0x80,0,0,2,0x11,L"游ゴシック");
    puVar2 = (undefined4 *)FUN_00414580(&DAT_005b66f0,7);
    *puVar2 = pHVar1;
    pHVar1 = CreateFontW(0x48,0,0,0,400,0,0,0,0x80,0,0,2,0x11,L"游ゴシック");
    puVar2 = (undefined4 *)FUN_00414580(&DAT_005b66f0,8);
    *puVar2 = pHVar1;
    pHVar1 = CreateFontW(0x5a,0,0,0,400,0,0,0,0x80,0,0,2,0x11,L"游ゴシック");
    puVar2 = (undefined4 *)FUN_00414580(&DAT_005b66f0,9);
    *puVar2 = pHVar1;
    pHVar1 = CreateFontW(0x60,0,0,0,400,0,0,0,0x80,0,0,2,0x11,L"游ゴシック");
    puVar2 = (undefined4 *)FUN_00414580(&DAT_005b66f0,10);
    *puVar2 = pHVar1;
    pHVar1 = CreateFontW(0x30,0,0,0,600,0,0,0,0x80,0,0,2,0x11,L"游ゴシック");
    puVar2 = (undefined4 *)FUN_00414580(&DAT_005b66f0,0xb);
    *puVar2 = pHVar1;
  }
  if (DAT_005b66ee == '\0') {
    pwStack_68 = L"ＭＳ 明朝";
  }
  else {
    pwStack_68 = L"Yu Mincho";
  }
  ReleaseDC((HWND)0x0,hdc);
  pHVar1 = CreateFontW(0x20,0,0,0,700,0,0,0,0x80,0,0,2,0x11,pwStack_68);
  puVar2 = (undefined4 *)FUN_00414580(&DAT_005b66f0,0xd);
  *puVar2 = pHVar1;
  pHVar1 = CreateFontW(0x28,0,0,0,700,0,0,0,0x80,0,0,2,0x11,pwStack_68);
  puVar2 = (undefined4 *)FUN_00414580(&DAT_005b66f0,0xe);
  *puVar2 = pHVar1;
  pHVar1 = CreateFontW(0x30,0,0,0,700,0,0,0,0x80,0,0,2,0x11,pwStack_68);
  puVar2 = (undefined4 *)FUN_00414580(&DAT_005b66f0,0xf);
  *puVar2 = pHVar1;
  pHVar1 = CreateFontW(0x3c,0,0,0,700,0,0,0,0x80,0,0,2,0x11,pwStack_68);
  puVar2 = (undefined4 *)FUN_00414580(&DAT_005b66f0,0x10);
  *puVar2 = pHVar1;
  pHVar1 = CreateFontW(0x40,0,0,0,700,0,0,0,0x80,0,0,2,0x11,pwStack_68);
  puVar2 = (undefined4 *)FUN_00414580(&DAT_005b66f0,0x11);
  *puVar2 = pHVar1;
  pHVar1 = CreateFontW(0xf,0,0,0,700,0,0,0,0x80,0,0,2,0x11,pwStack_68);
  puVar2 = (undefined4 *)FUN_00414580(&DAT_005b66f0,0x14);
  *puVar2 = pHVar1;
  pHVar1 = CreateFontW(0xf,0,0,0,700,0,0,0,0x80,0,0,2,0x11,pwStack_68);
  puVar2 = (undefined4 *)FUN_00414580(&DAT_005b66f0,0x15);
  *puVar2 = pHVar1;
  pHVar1 = CreateFontW(0xf,0,0,0,400,0,0,0,0x80,0,0,2,0x11,pWStack_6c);
  puVar2 = (undefined4 *)FUN_00414580(&DAT_005b66f0,0x12);
  *puVar2 = pHVar1;
  pHVar1 = CreateFontW(0xf,0,0,0,400,0,0,0,0x80,0,0,2,0x11,pWStack_6c);
  puVar2 = (undefined4 *)FUN_00414580(&DAT_005b66f0,0x13);
  *puVar2 = pHVar1;
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

