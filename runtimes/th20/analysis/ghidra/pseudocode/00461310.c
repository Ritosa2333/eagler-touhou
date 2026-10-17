/* Entry: 0x00461310; symbol: FUN_00461310; body bytes: 133 */

undefined4 __thiscall FUN_00461310(void *this,undefined4 *param_1)

{
  *(undefined4 *)((int)this + 8) = *param_1;
  *(undefined4 *)((int)this + 0xc) = param_1[1];
  *(undefined4 *)((int)this + 0x10) = param_1[2];
  *(undefined4 *)((int)this + 0x14) = param_1[3];
  *(undefined4 *)((int)this + 0x18) = param_1[4];
  *(undefined4 *)((int)this + 0x1c) = param_1[5];
  *(undefined4 *)((int)this + 0x20) = param_1[8];
  FUN_00423520((void *)((int)this + 0x28),0);
  FUN_00450350(*(void **)((int)this + 4),0x23);
  FUN_004614e0(*(void **)((int)this + 4),2);
  return 0;
}

