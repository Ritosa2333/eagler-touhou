/* Entry: 0x004f3eb0; symbol: FUN_004f3eb0; body bytes: 90 */

GdiplusStartupInput * __thiscall FUN_004f3eb0(void *this,undefined4 param_1,undefined4 param_2)

{
  undefined4 *puVar1;
  
  FUN_0040b120((int)this + 0x10);
  Gdiplus::GdiplusStartupInput::GdiplusStartupInput
            ((GdiplusStartupInput *)this,(_func_void_DebugEventLevel_char_ptr *)((int)this + 0x14),0
             ,0x100);
  *(undefined ***)this = &PTR_FUN_00572ca8;
  puVar1 = (undefined4 *)move<>(&param_1);
  *(undefined4 *)((int)this + 0x10) = *puVar1;
  return (GdiplusStartupInput *)this;
}

