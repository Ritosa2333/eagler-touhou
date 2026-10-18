/* Entry: 0x00497590; symbol: FUN_00497590; body bytes: 75 */

undefined4 * __thiscall FUN_00497590(void *this,undefined4 *param_1)

{
  undefined4 *puVar1;
  
  puVar1 = *(undefined4 **)((int)this + 4);
  if (puVar1 != (undefined4 *)0x0) {
    FUN_0040cd70(puVar1,param_1);
    *param_1 = *(undefined4 *)((int)this + 8);
    *(undefined4 *)((int)this + 4) = 0;
    param_1 = puVar1;
  }
  return param_1;
}

