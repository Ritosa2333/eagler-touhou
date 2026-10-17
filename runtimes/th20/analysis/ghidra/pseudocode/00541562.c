/* Entry: 0x00541562; symbol: FUN_00541562; body bytes: 25 */

undefined ** __cdecl FUN_00541562(undefined4 param_1)

{
  undefined **ppuVar1;
  undefined **ppuVar2;
  
  ppuVar1 = DAT_005e4d28;
  LOCK();
  DAT_005e4d28 = (undefined **)param_1;
  UNLOCK();
  ppuVar2 = &PTR_PTR_005b2578;
  if (ppuVar1 != (undefined **)0x0) {
    ppuVar2 = ppuVar1;
  }
  return ppuVar2;
}

