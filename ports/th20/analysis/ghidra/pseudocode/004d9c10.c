/* Entry: 0x004d9c10; symbol: FUN_004d9c10; body bytes: 46 */

void __thiscall FUN_004d9c10(void *this,undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  Status SVar1;
  
  SVar1 = GdipSaveImageToFile(*(undefined4 *)((int)this + 4),param_1,param_2,param_3);
  Gdiplus::Image::SetStatus((Image *)this,SVar1);
  return;
}

