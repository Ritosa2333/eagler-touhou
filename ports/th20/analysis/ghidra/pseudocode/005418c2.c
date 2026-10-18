/* Entry: 0x005418c2; symbol: FUN_005418c2; body bytes: 62 */

void __cdecl FUN_005418c2(undefined8 *param_1)

{
  undefined8 uVar1;
  undefined8 uVar2;
  uint uVar3;
  
  uVar1 = __Xtime_get_ticks();
  uVar3 = (uint)((ulonglong)uVar1 >> 0x20);
  uVar2 = __aulldiv((uint)uVar1,uVar3,10000000,0);
  *param_1 = uVar2;
  uVar1 = __aullrem((uint)uVar1,uVar3,10000000,0);
  *(int *)(param_1 + 1) = (int)uVar1 * 100;
  return;
}

