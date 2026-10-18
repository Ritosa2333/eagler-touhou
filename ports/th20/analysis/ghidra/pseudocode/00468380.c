/* Entry: 0x00468380; symbol: FUN_00468380; body bytes: 45 */

void __cdecl FUN_00468380(undefined4 param_1,byte param_2,size_t param_3)

{
  void *_Dst;
  uint _Val;
  size_t _Size;
  
  _Val = (uint)param_2;
  _Size = param_3;
  _Dst = (void *)FUN_00425620(&param_1);
  _memset(_Dst,_Val,_Size);
  return;
}

