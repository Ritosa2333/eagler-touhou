/* Entry: 0x0041ae70; symbol: FUN_0041ae70; body bytes: 1546 */

void FUN_0041ae70(void)

{
  bool bVar1;
  ushort uVar2;
  undefined3 extraout_var;
  HINSTANCE pHVar3;
  GpImage *pGVar4;
  UINT UVar5;
  LPCWSTR pWVar6;
  HWND pHVar7;
  code *pcVar8;
  int iVar9;
  LPARAM LVar10;
  BOOL BVar11;
  tagMSG *lpMsg;
  char **ppcVar12;
  int local_114;
  int local_110;
  uint local_10c;
  int local_108;
  int local_104;
  DEVMODEW local_100;
  tagMSG local_24;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  ppcVar12 = (char **)0x0;
  if ((((DAT_005c4f8c >> 7 & 1) != 0) || (uVar2 = GetKeyState(0x10), (uVar2 & 0x80) != 0)) ||
     (uVar2 = GetKeyState(0x11), (uVar2 & 0x80) != 0)) {
    local_104 = 0;
    bVar1 = FUN_0041d0c0();
    if (CONCAT31(extraout_var,bVar1) == 0) {
      LVar10 = 0;
      pcVar8 = FUN_0041abe0;
      pHVar7 = (HWND)0x0;
      pWVar6 = (LPCWSTR)0xcc;
      pHVar3 = (HINSTANCE)FUN_0041cab0(0x5b6758);
      pGVar4 = (GpImage *)CreateDialogParamW(pHVar3,pWVar6,pHVar7,pcVar8,LVar10);
      Gdiplus::Image::SetNativeImage((Image *)&DAT_005b6758,pGVar4);
    }
    else {
      LVar10 = 0;
      pcVar8 = FUN_0041abe0;
      pHVar7 = (HWND)0x0;
      pWVar6 = (LPCWSTR)0xcb;
      pHVar3 = (HINSTANCE)FUN_0041cab0(0x5b6758);
      pGVar4 = (GpImage *)CreateDialogParamW(pHVar3,pWVar6,pHVar7,pcVar8,LVar10);
      Gdiplus::Image::SetNativeImage((Image *)&DAT_005b6758,pGVar4);
    }
    _memset(&local_100,0,0xdc);
    local_100.dmPelsWidth = 0;
    EnumDisplaySettingsW((LPCWSTR)0x0,0xffffffff,&local_100);
    if ((local_100.dmPelsWidth < 0xa00) || (local_100.dmPelsHeight < 0x780)) {
      BVar11 = 0;
      iVar9 = 0xd4;
      pHVar7 = (HWND)FUN_0040ff90(0x5b6758);
      pHVar7 = GetDlgItem(pHVar7,iVar9);
      EnableWindow(pHVar7,BVar11);
    }
    if ((local_100.dmPelsWidth < 0x780) && (local_100.dmPelsHeight < 0x5a0)) {
      BVar11 = 0;
      iVar9 = 0xd5;
      pHVar7 = (HWND)FUN_0040ff90(0x5b6758);
      pHVar7 = GetDlgItem(pHVar7,iVar9);
      EnableWindow(pHVar7,BVar11);
    }
    if ((local_100.dmPelsWidth < 0x500) && (local_100.dmPelsHeight < 0x3c0)) {
      BVar11 = 0;
      iVar9 = 0xd6;
      pHVar7 = (HWND)FUN_0040ff90(0x5b6758);
      pHVar7 = GetDlgItem(pHVar7,iVar9);
      EnableWindow(pHVar7,BVar11);
      BVar11 = 0;
      iVar9 = 0xd1;
      pHVar7 = (HWND)FUN_0040ff90(0x5b6758);
      pHVar7 = GetDlgItem(pHVar7,iVar9);
      EnableWindow(pHVar7,BVar11);
    }
    if ((local_100.dmPelsWidth < 0x3c0) && (local_100.dmPelsHeight < 0x2d0)) {
      BVar11 = 0;
      iVar9 = 0xd7;
      pHVar7 = (HWND)FUN_0040ff90(0x5b6758);
      pHVar7 = GetDlgItem(pHVar7,iVar9);
      EnableWindow(pHVar7,BVar11);
      BVar11 = 0;
      iVar9 = 0xd2;
      pHVar7 = (HWND)FUN_0040ff90(0x5b6758);
      pHVar7 = GetDlgItem(pHVar7,iVar9);
      EnableWindow(pHVar7,BVar11);
    }
    for (local_108 = 0; local_108 < 10; local_108 = local_108 + 1) {
      iVar9 = *(int *)(&DAT_005ae1f8 + local_108 * 4);
      pHVar7 = (HWND)FUN_0040ff90(0x5b6758);
      UVar5 = IsDlgButtonChecked(pHVar7,iVar9);
      if (UVar5 == 1) {
        local_104 = local_108;
        break;
      }
    }
    pHVar7 = (HWND)FUN_0040ff90(0x5b6758);
    BVar11 = IsWindow(pHVar7);
    if (BVar11 != 0) {
      iVar9 = 5;
      pHVar7 = (HWND)FUN_0040ff90(0x5b6758);
      ShowWindow(pHVar7,iVar9);
LAB_0041b1da:
      FUN_00420580();
      BVar11 = PeekMessageW(&local_24,(HWND)0x0,0,0,1);
      if (BVar11 != 0) goto code_r0x0041b203;
      goto LAB_0041b233;
    }
    std::_Adl_verify_range<char*,char_const*>((char **)"Invalid window handle.\n",ppcVar12);
  }
LAB_0041b46c:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
code_r0x0041b203:
  lpMsg = &local_24;
  pHVar7 = (HWND)FUN_0040ff90(0x5b6758);
  BVar11 = IsDialogMessageW(pHVar7,lpMsg);
  if (BVar11 == 0) {
    TranslateMessage(&local_24);
    DispatchMessageW(&local_24);
LAB_0041b233:
    if (DAT_005b88a4 == (void *)0x0) {
      local_10c = 0;
    }
    else {
      local_10c = FUN_00419c00(DAT_005b88a4,0x80001);
    }
    if (local_10c == 0) {
      if (DAT_005b88a4 == (void *)0x0) {
        local_110 = 0;
      }
      else {
        local_110 = FUN_0041a280(DAT_005b88a4,0x20);
      }
      if ((local_110 != 0) && (local_104 < 9)) {
        do {
          local_104 = local_104 + 1;
        } while (*(int *)(&stack0xfffffea0 + local_104 * 4) != 0);
        iVar9 = *(int *)(&DAT_005ae1f8 + local_104 * 4);
        pHVar7 = (HWND)FUN_0040ff90(0x5b6758);
        CheckRadioButton(pHVar7,0xcf,0xd8,iVar9);
      }
      if (DAT_005b88a4 == (void *)0x0) {
        local_114 = 0;
      }
      else {
        local_114 = FUN_0041a280(DAT_005b88a4,0x10);
      }
      if ((local_114 != 0) && (0 < local_104)) {
        do {
          local_104 = local_104 + -1;
        } while (*(int *)(&stack0xfffffea0 + local_104 * 4) != 0);
        iVar9 = *(int *)(&DAT_005ae1f8 + local_104 * 4);
        pHVar7 = (HWND)FUN_0040ff90(0x5b6758);
        CheckRadioButton(pHVar7,0xcf,0xd8,iVar9);
      }
      iVar9 = FUN_0040ff90(0x5b6758);
      if (iVar9 == 0) goto LAB_0041b46c;
      Sleep(6);
      goto LAB_0041b1da;
    }
    FUN_0041ab30();
    DAT_005b87e8 = DAT_005b87e8 & 0xffffffe7;
    pHVar7 = (HWND)FUN_0040ff90(0x5b6758);
    DestroyWindow(pHVar7);
    Gdiplus::Image::SetNativeImage((Image *)&DAT_005b6758,(GpImage *)0x0);
    goto LAB_0041b46c;
  }
  goto LAB_0041b1da;
}

