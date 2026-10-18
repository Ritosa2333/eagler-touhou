/* Entry: 0x00534600; symbol: FUN_00534600; body bytes: 211 */

void __thiscall FUN_00534600(void *this,int param_1,GpImage *param_2)

{
  void *pvVar1;
  undefined4 uVar2;
  
  FUN_00532740(*(void **)((int)this + 0x34));
  *(undefined4 *)((int)this + 0x34) = 0;
  pvVar1 = (void *)FUN_00464080(0);
  FUN_005346e0(pvVar1,0);
  if (param_2 == (GpImage *)0x8) {
    pvVar1 = (void *)FUN_00464080(0);
    param_2 = (GpImage *)FUN_0050a280(pvVar1,0);
    pvVar1 = (void *)FUN_00464080(0);
    FUN_005346e0(pvVar1,1);
  }
  pvVar1 = (void *)FUN_00464080(0);
  FUN_0041de90(pvVar1,param_2);
  uVar2 = (*(code *)(&PTR_FUN_005b0ab0)[(int)(param_2 + param_1 * 9)])();
  *(undefined4 *)((int)this + 0x34) = uVar2;
  FUN_00412dc0(*(void **)((int)this + 0x34),3);
  Gdiplus::Image::SetNativeImage(*(Image **)((int)this + 0x34),param_2);
  return;
}

