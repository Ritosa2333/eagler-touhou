/* Entry: 0x004a0fb0; symbol: FUN_004a0fb0; body bytes: 57 */

void __thiscall FUN_004a0fb0(void *this,undefined4 param_1)

{
  if ((*(uint *)((int)this + 0xb48) >> 9 & 1) == 0) {
    *(undefined4 *)((int)this + 0xb0c) = param_1;
  }
  else {
    *(undefined4 *)((int)this + 0xb0c) = 2;
  }
  return;
}

