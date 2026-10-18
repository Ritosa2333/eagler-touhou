/* Entry: 0x0052f900; symbol: FUN_0052f900; body bytes: 163 */

undefined4 * __cdecl FUN_0052f900(uint param_1)

{
  int iVar1;
  undefined4 *puVar2;
  
  iVar1 = FUN_0052ca70(DAT_005c6108,param_1);
  if (iVar1 == 0) {
    FUN_0052f840(DAT_005c6108,param_1);
    if (DAT_005c6858 == (undefined4 *)0x0) {
      puVar2 = FUN_0052d690();
      if (puVar2 == (undefined4 *)0x0) {
        puVar2 = (undefined4 *)0x0;
      }
      else {
        iVar1 = FUN_0052e170(puVar2);
        if (iVar1 != 0) {
          if (puVar2 != (undefined4 *)0x0) {
            FUN_0041f7c0(puVar2);
          }
          puVar2 = (undefined4 *)0x0;
        }
      }
    }
    else {
      FUN_0052df00((int)DAT_005c6858);
      puVar2 = DAT_005c6858;
    }
  }
  else {
    puVar2 = (undefined4 *)0x0;
  }
  return puVar2;
}

