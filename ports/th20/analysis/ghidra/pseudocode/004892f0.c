/* Entry: 0x004892f0; symbol: FUN_004892f0; body bytes: 80 */

undefined4 * __thiscall FUN_004892f0(void *this,undefined4 param_1)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  
                    /* WARNING: Load size is inaccurate */
  puVar1 = (undefined4 *)move<>(0x14,*this);
  puVar2 = (undefined4 *)move<>(param_1);
  *puVar1 = *puVar2;
  puVar1[1] = puVar2[1];
  puVar1[2] = puVar2[2];
  puVar1[3] = puVar2[3];
  puVar1[4] = puVar2[4];
  return puVar1;
}

