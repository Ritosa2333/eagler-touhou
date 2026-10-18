/* Entry: 0x00506a50; symbol: FUN_00506a50; body bytes: 30 */

void __thiscall FUN_00506a50(void *this,undefined4 *param_1)

{
  undefined4 uVar1;
  
  uVar1 = param_1[1];
  *(undefined4 *)((int)this + 0x2c) = *param_1;
  *(undefined4 *)((int)this + 0x30) = uVar1;
  return;
}

