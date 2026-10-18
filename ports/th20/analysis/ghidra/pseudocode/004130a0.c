/* Entry: 0x004130a0; symbol: FUN_004130a0; body bytes: 62 */

undefined4 * __thiscall FUN_004130a0(void *this,undefined4 param_1)

{
  undefined4 uVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  
                    /* WARNING: Load size is inaccurate */
  puVar2 = (undefined4 *)move<>(8,*this);
  puVar3 = (undefined4 *)move<>(param_1);
  uVar1 = puVar3[1];
  *puVar2 = *puVar3;
  puVar2[1] = uVar1;
  return puVar2;
}

