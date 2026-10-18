/* Entry: 0x00450350; symbol: FUN_00450350; body bytes: 217 */

void __thiscall FUN_00450350(void *this,undefined4 param_1)

{
  *(undefined4 *)((int)this + 0x14) = param_1;
  FUN_0040e5e0();
  if ((*(int *)((int)this + 0x14) < 3) || (0x13 < *(int *)((int)this + 0x14))) {
    if ((*(int *)((int)this + 0x14) < 0x14) || (0x17 < *(int *)((int)this + 0x14))) {
      *(byte *)((int)this + 0x4a3) = *(byte *)((int)this + 0x4a3) & 0xfc;
    }
    else {
      *(byte *)((int)this + 0x4a3) = *(byte *)((int)this + 0x4a3) & 0xfc | 2;
    }
  }
  else {
    *(byte *)((int)this + 0x4a3) = *(byte *)((int)this + 0x4a3) & 0xfc | 1;
  }
  if ((((0x13 < *(int *)((int)this + 0x14)) && (*(int *)((int)this + 0x14) < 0x25)) ||
      ((0x2c < *(int *)((int)this + 0x14) && (*(int *)((int)this + 0x14) < 0x36)))) &&
     ((*(uint *)((int)this + 0x49c) >> 0x17 & 1) == 0)) {
    *(undefined1 *)((int)this + 0x4a4) = 1;
  }
  return;
}

