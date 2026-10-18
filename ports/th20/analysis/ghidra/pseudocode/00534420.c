/* Entry: 0x00534420; symbol: FUN_00534420; body bytes: 131 */

void __thiscall FUN_00534420(void *this,int param_1,GpImage *param_2)

{
  void *this_00;
  undefined4 uVar1;
  
  FUN_00532740(*(void **)((int)this + 0x28));
  *(undefined4 *)((int)this + 0x28) = 0;
  this_00 = (void *)FUN_00464080(0);
  FUN_00412dc0(this_00,param_2);
  uVar1 = (*(code *)(&PTR_FUN_005b0ab0)[(int)(param_2 + param_1 * 9)])();
  *(undefined4 *)((int)this + 0x28) = uVar1;
  FUN_00412dc0(*(void **)((int)this + 0x28),0);
  Gdiplus::Image::SetNativeImage(*(Image **)((int)this + 0x28),param_2);
  return;
}

