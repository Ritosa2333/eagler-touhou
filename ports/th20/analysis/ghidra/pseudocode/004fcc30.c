/* Entry: 0x004fcc30; symbol: FUN_004fcc30; body bytes: 46 */

undefined4 * __thiscall FUN_004fcc30(void *this,undefined4 *param_1)

{
  undefined4 *puVar1;
  
  FUN_004fbeb0((int)this);
  puVar1 = (undefined4 *)move<>((int)this + 0x10);
  *param_1 = *puVar1;
  return param_1;
}

