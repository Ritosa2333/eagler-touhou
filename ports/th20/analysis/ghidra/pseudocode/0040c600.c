/* Entry: 0x0040c600; symbol: FUN_0040c600; body bytes: 53 */

void __fastcall FUN_0040c600(undefined4 param_1)

{
  undefined1 uVar1;
  undefined1 *puVar2;
  
  puVar2 = (undefined1 *)move<>(param_1);
  uVar1 = FUN_0040b290(&stack0x00000004);
  LOCK();
  *puVar2 = uVar1;
  UNLOCK();
  return;
}

