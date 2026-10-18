/* Entry: 0x004fe800; symbol: FUN_004fe800; body bytes: 68 */

ulonglong __cdecl
FUN_004fe800(undefined4 param_1,uint param_2,undefined4 param_3,int param_4,int param_5)

{
  ulonglong uVar1;
  ulonglong uVar2;
  
  if (param_5 == 0) {
    uVar1 = CONCAT44(param_2,param_1);
  }
  else {
    uVar1 = __allshl(0x40 - (byte)param_5,param_4);
    uVar2 = __aullshr((byte)param_5,param_2);
    uVar1 = uVar1 | uVar2;
  }
  return uVar1;
}

