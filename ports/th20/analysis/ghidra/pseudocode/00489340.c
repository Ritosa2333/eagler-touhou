/* Entry: 0x00489340; symbol: FUN_00489340; body bytes: 68 */

undefined4 * __thiscall FUN_00489340(void *this,undefined4 param_1)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  int iVar3;
  undefined4 *puVar4;
  
                    /* WARNING: Load size is inaccurate */
  puVar1 = (undefined4 *)move<>(0x184,*this);
  puVar2 = (undefined4 *)move<>(param_1);
  puVar4 = puVar1;
  for (iVar3 = 0x61; iVar3 != 0; iVar3 = iVar3 + -1) {
    *puVar4 = *puVar2;
    puVar2 = puVar2 + 1;
    puVar4 = puVar4 + 1;
  }
  return puVar1;
}

