/* Entry: 0x005347d0; symbol: FUN_005347d0; body bytes: 211 */

void __thiscall FUN_005347d0(void *this,int param_1,GpImage *param_2)

{
  void *pvVar1;
  undefined4 uVar2;
  
  FUN_00532740(*(void **)((int)this + 0x30));
  *(undefined4 *)((int)this + 0x30) = 0;
  pvVar1 = (void *)FUN_00464080(0);
  FUN_005348b0(pvVar1,0);
  if (param_2 == (GpImage *)0x8) {
    pvVar1 = (void *)FUN_00464080(0);
    param_2 = (GpImage *)FUN_0050a280(pvVar1,0);
    pvVar1 = (void *)FUN_00464080(0);
    FUN_005348b0(pvVar1,1);
  }
  pvVar1 = (void *)FUN_00464080(0);
  FUN_0041de50(pvVar1,param_2);
  uVar2 = (*(code *)(&PTR_FUN_005b0ab0)[(int)(param_2 + param_1 * 9)])();
  *(undefined4 *)((int)this + 0x30) = uVar2;
  FUN_00412dc0(*(void **)((int)this + 0x30),1);
  Gdiplus::Image::SetNativeImage(*(Image **)((int)this + 0x30),param_2);
  return;
}

