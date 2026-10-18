/* Entry: 0x0041abe0; symbol: FUN_0041abe0; body bytes: 651 */

void FUN_0041abe0(HWND param_1,uint param_2,uint param_3,HRAWINPUT param_4)

{
  int *pData;
  UINT UVar1;
  HWND pHVar2;
  BOOL BVar3;
  WPARAM WVar4;
  LPARAM LVar5;
  uint local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  if (param_2 < 0x111) {
    if (param_2 == 0x110) {
      if ((DAT_005c4f8c >> 7 & 1) != 0) {
        LVar5 = 0;
        WVar4 = 1;
        UVar1 = 0xf1;
        pHVar2 = GetDlgItem(param_1,0xca);
        SendMessageW(pHVar2,UVar1,WVar4,LVar5);
      }
      LVar5 = 0;
      WVar4 = 1;
      UVar1 = 0xf1;
      pHVar2 = GetDlgItem(param_1,*(int *)(&DAT_005ae128 + DAT_005c4f80 * 4));
      SendMessageW(pHVar2,UVar1,WVar4,LVar5);
      DAT_005b87e8 = DAT_005b87e8 & 0xffffffe7 | 0x10;
      FUN_0041a1b0(param_1);
      goto LAB_0041ae5f;
    }
    if (param_2 == 0x10) {
      if ((DAT_005b87e8 >> 3 & 3) == 2) {
        DAT_005b87e8 = DAT_005b87e8 & 0xffffffe7 | 8;
      }
      pHVar2 = (HWND)FUN_0040ff90(0x5b6758);
      DestroyWindow(pHVar2);
      Gdiplus::Image::SetNativeImage((Image *)&DAT_005b6758,(GpImage *)0x0);
      goto LAB_0041ae5f;
    }
    if (param_2 == 0xff) {
      GetRawInputData(param_4,0x10000003,(LPVOID)0x0,&local_c,0x10);
      pData = (int *)FUN_00542a01(local_c);
      UVar1 = GetRawInputData(param_4,0x10000003,pData,&local_c,0x10);
      if (((UVar1 != local_c) || (*pData != 2)) || (*(char *)((int)pData + 0x19) == '\0')) {
        FUN_0054278d(pData);
        goto LAB_0041ae5f;
      }
      FUN_0054278d(pData);
      goto LAB_0041adac;
    }
  }
  else if (param_2 == 0x111) {
    if ((param_3 & 0xffff) != 0xce) goto LAB_0041ae5f;
LAB_0041adac:
    FUN_0041ab30();
    DAT_005b87e8 = DAT_005b87e8 & 0xffffffe7;
    pHVar2 = (HWND)FUN_0040ff90(0x5b6758);
    BVar3 = IsWindow(pHVar2);
    if (BVar3 != 0) {
      pHVar2 = (HWND)FUN_0040ff90(0x5b6758);
      DestroyWindow(pHVar2);
    }
    Gdiplus::Image::SetNativeImage((Image *)&DAT_005b6758,(GpImage *)0x0);
    goto LAB_0041ae5f;
  }
  DefWindowProcW(param_1,param_2,param_3,(LPARAM)param_4);
LAB_0041ae5f:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

