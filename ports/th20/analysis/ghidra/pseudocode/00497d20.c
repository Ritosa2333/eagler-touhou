/* Entry: 0x00497d20; symbol: FUN_00497d20; body bytes: 79 */

uint __thiscall FUN_00497d20(void *this,undefined4 param_1)

{
  uint uVar1;
  undefined4 *puVar2;
  
  uVar1 = FUN_00498e70((int)this);
  if ((uVar1 == 0xffffffff) || (0x3ff < (int)uVar1)) {
    uVar1 = 0;
  }
  else {
    puVar2 = (undefined4 *)FUN_00414580((void *)((int)this + 0x3c),uVar1);
    *puVar2 = param_1;
    uVar1 = uVar1 | 0x80000000;
  }
  return uVar1;
}

