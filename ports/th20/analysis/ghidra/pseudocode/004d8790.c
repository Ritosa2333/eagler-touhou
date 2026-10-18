/* Entry: 0x004d8790; symbol: FUN_004d8790; body bytes: 147 */

void __thiscall FUN_004d8790(void *this,undefined4 param_1,undefined4 param_2)

{
  undefined4 uVar1;
  int *unaff_FS_OFFSET;
  GpImage *local_18;
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00569c8d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_004d88a0((undefined4 *)this);
  local_8 = 0;
  *(undefined ***)this = &PTR_FID_conflict__scalar_deleting_destructor__00571920;
  local_18 = (GpImage *)0x0;
  uVar1 = GdipCreateBitmapFromHBITMAP(param_1,param_2,&local_18);
  *(undefined4 *)((int)this + 8) = uVar1;
  Gdiplus::Image::SetNativeImage((Image *)this,local_18);
  local_8 = 0xffffffff;
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

