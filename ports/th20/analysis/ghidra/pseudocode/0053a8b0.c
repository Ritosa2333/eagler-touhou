/* Entry: 0x0053a8b0; symbol: FUN_0053a8b0; body bytes: 65 */

undefined4 * __thiscall FUN_0053a8b0(void *this,undefined4 param_1)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  int iVar3;
  undefined4 *puVar4;
  
                    /* WARNING: Load size is inaccurate */
  puVar1 = (undefined4 *)move<>(0x38,*this);
  puVar2 = (undefined4 *)move<>(param_1);
  puVar4 = puVar1;
  for (iVar3 = 0xe; iVar3 != 0; iVar3 = iVar3 + -1) {
    *puVar4 = *puVar2;
    puVar2 = puVar2 + 1;
    puVar4 = puVar4 + 1;
  }
  return puVar1;
}

