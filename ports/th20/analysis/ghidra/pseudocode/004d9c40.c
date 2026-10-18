/* Entry: 0x004d9c40; symbol: FUN_004d9c40; body bytes: 363 */

void __cdecl FUN_004d9c40(undefined4 param_1,LPCSTR param_2)

{
  undefined4 *this;
  int iVar1;
  undefined4 uVar2;
  int *unaff_FS_OFFSET;
  undefined4 local_248 [4];
  GdiplusStartupInput local_238 [16];
  undefined4 local_228;
  undefined4 local_224;
  WCHAR local_220 [262];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00569d40;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  Gdiplus::GdiplusStartupInput::GdiplusStartupInput
            (local_238,(_func_void_DebugEventLevel_char_ptr *)0x0,0,0);
  GdiplusStartup(&local_228,local_238,0);
  this = (undefined4 *)FUN_004d9760(param_1,0);
  _memset(local_220,0,0x20a);
  MultiByteToWideChar(0x3a4,0,param_2,-1,local_220,0x104);
  iVar1 = FUN_004d97f0((ushort *)L"image/png",local_248);
  if (iVar1 != 0) {
    uVar2 = FUN_0040c310(&DAT_005c0240,2);
    FUN_0040b730(&local_224,uVar2);
    local_8 = 0;
    FUN_004d9c10(this,local_220,local_248,0);
    local_8 = 0xffffffff;
    FUN_0040b900(&local_224);
  }
  if (this != (undefined4 *)0x0) {
    (**(code **)*this)(1);
  }
  GdiplusShutdown(local_228);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

