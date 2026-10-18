/* Entry: 0x0040c550; symbol: FUN_0040c550; body bytes: 53 */

void __fastcall FUN_0040c550(undefined4 param_1)

{
  undefined4 *puVar1;
  undefined4 uVar2;
  
  puVar1 = (undefined4 *)move<>(param_1);
  uVar2 = FUN_0040b2a0((undefined4 *)&stack0x00000004);
  LOCK();
  *puVar1 = uVar2;
  UNLOCK();
  return;
}

