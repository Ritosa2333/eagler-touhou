/* Entry: 0x0046ff30; symbol: FUN_0046ff30; body bytes: 71 */

bool __cdecl FUN_0046ff30(uint param_1,uint param_2,byte param_3)

{
  longlong lVar1;
  bool local_8;
  
  lVar1 = __allshl(param_3,0);
  local_8 = ((int)lVar1 - 1U & param_1) == 0 &&
            ((int)((ulonglong)lVar1 >> 0x20) - (uint)((int)lVar1 == 0) & param_2) == 0;
  return local_8;
}

