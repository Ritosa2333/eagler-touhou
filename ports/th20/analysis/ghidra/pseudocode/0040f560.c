/* Entry: 0x0040f560; symbol: FUN_0040f560; body bytes: 270 */

void __thiscall FUN_0040f560(void *this,uint param_1,int param_2)

{
  bool bVar1;
  uint uVar2;
  uint local_10;
  
  if (param_1 != 0) {
    Gdiplus::Image::SetNativeImage((Image *)this,(GpImage *)0xffff);
    uVar2 = FUN_00411810(param_1);
    local_10 = (uint)((uVar2 & 0xff) != 0);
    FUN_004117a0(this,local_10);
    return;
  }
  uVar2 = *(uint *)(param_2 + 0x10);
  bVar1 = FUN_0040c9b0(uVar2,1);
  if (bVar1) {
    Gdiplus::Image::SetNativeImage((Image *)this,(GpImage *)0x16d);
  }
  else {
    Gdiplus::Image::SetNativeImage((Image *)this,(GpImage *)0x1ff);
  }
  bVar1 = FUN_0040c9b0(uVar2,0x400);
  if (bVar1) {
    if (*(int *)(param_2 + 0x14) == -0x5ffffff4) {
      FUN_004117a0(this,4);
      return;
    }
    if (*(int *)(param_2 + 0x14) == -0x5ffffffd) {
      FUN_004117a0(this,10);
      return;
    }
  }
  bVar1 = FUN_0040c9b0(uVar2,0x10);
  if (bVar1) {
    FUN_004117a0(this,3);
  }
  else {
    FUN_004117a0(this,2);
  }
  return;
}

