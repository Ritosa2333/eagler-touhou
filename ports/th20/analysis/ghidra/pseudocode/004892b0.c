/* Entry: 0x004892b0; symbol: FUN_004892b0; body bytes: 56 */

undefined4 * __thiscall FUN_004892b0(void *this,undefined4 param_1)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  
                    /* WARNING: Load size is inaccurate */
  puVar1 = (undefined4 *)move<>(4,*this);
  puVar2 = (undefined4 *)move<>(param_1);
  *puVar1 = *puVar2;
  return puVar1;
}

