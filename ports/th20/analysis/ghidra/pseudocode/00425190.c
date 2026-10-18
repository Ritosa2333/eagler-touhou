/* Entry: 0x00425190; symbol: FUN_00425190; body bytes: 74 */

undefined4 * __thiscall FUN_00425190(void *this,undefined4 param_1)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  
                    /* WARNING: Load size is inaccurate */
  puVar1 = (undefined4 *)move<>(0x10,*this);
  puVar2 = (undefined4 *)move<>(param_1);
  *puVar1 = *puVar2;
  puVar1[1] = puVar2[1];
  puVar1[2] = puVar2[2];
  puVar1[3] = puVar2[3];
  return puVar1;
}

