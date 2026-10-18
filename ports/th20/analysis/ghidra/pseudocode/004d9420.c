/* Entry: 0x004d9420; symbol: FUN_004d9420; body bytes: 184 */

void __fastcall FUN_004d9420(Image *param_1)

{
  Status SVar1;
  Image *this;
  int *unaff_FS_OFFSET;
  GpImage *local_18;
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00569cd0;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  local_18 = (GpImage *)0x0;
  SVar1 = GdipCloneImage(*(undefined4 *)(param_1 + 4),&local_18);
  Gdiplus::Image::SetStatus(param_1,SVar1);
  this = (Image *)FUN_004d8dd0(0x10);
  local_8 = 0;
  if (this != (Image *)0x0) {
    FUN_0040c080(this,0x10);
    Gdiplus::Image::Image(this,local_18,*(Status *)(param_1 + 8));
  }
  local_8 = 0xffffffff;
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

