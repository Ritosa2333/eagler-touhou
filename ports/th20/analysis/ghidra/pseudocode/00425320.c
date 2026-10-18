/* Entry: 0x00425320; symbol: FUN_00425320; body bytes: 35 */

void __cdecl FUN_00425320(undefined4 param_1,int param_2)

{
  size_t _Size;
  void *_Dst;
  int _Val;
  
  _Size = param_2 << 2;
  _Val = 0;
  _Dst = (void *)FUN_00425620(&param_1);
  _memset(_Dst,_Val,_Size);
  return;
}

