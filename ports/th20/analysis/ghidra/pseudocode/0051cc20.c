/* Entry: 0x0051cc20; symbol: FUN_0051cc20; body bytes: 89 */

undefined4 * __cdecl FUN_0051cc20(undefined4 param_1)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  int iVar3;
  
  puVar2 = FUN_00515250();
  iVar3 = FUN_00519960(param_1);
  puVar1 = puVar2;
  if (iVar3 != 0) {
    if (puVar2 != (undefined4 *)0x0) {
      FUN_0041f7c0(puVar2);
    }
    puVar2 = (undefined4 *)0x0;
    puVar1 = DAT_005c6120;
  }
  DAT_005c6120 = puVar1;
  return puVar2;
}

