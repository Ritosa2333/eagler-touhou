/* Entry: 0x0041c320; symbol: FUN_0041c320; body bytes: 180 */

void FUN_0041c320(void)

{
  GpImage *pGVar1;
  int iVar2;
  DEVMODEW local_e4;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  pGVar1 = (GpImage *)Direct3DCreate9(0x20);
  Gdiplus::Image::SetNativeImage((Image *)&DAT_005c4d40,pGVar1);
  iVar2 = FUN_0040ff90(0x5c4d40);
  if (iVar2 == 0) {
    FUN_00454230(&DAT_005c0678,
                 L"楄敲瑣䐳茠草荵荗荆荎艧觍貽苌趩郬辬靯芈苈芩苁ල\n");
  }
  else {
    _memset(&local_e4,0,0xdc);
    local_e4.dmPelsWidth = 0;
    EnumDisplaySettingsW((LPCWSTR)0x0,0xffffffff,&local_e4);
    DAT_005b8808 = local_e4.dmPelsWidth;
    DAT_005b880c = local_e4.dmPelsHeight;
    DAT_005b8800 = local_e4.dmPelsWidth;
    DAT_005b8804 = local_e4.dmPelsHeight;
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

