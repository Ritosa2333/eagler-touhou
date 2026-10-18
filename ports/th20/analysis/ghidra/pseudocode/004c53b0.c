/* Entry: 0x004c53b0; symbol: FUN_004c53b0; body bytes: 64 */

undefined4 __cdecl FUN_004c53b0(undefined4 param_1,undefined4 param_2)

{
  void *_Dst;
  int iVar1;
  
  _Dst = (void *)FUN_00425620(&param_1);
  iVar1 = FUN_00425620(&param_2);
  _memset(_Dst,0,iVar1 - (int)_Dst);
  return param_2;
}

