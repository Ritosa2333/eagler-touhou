/* Entry: 0x00411f30; symbol: FUN_00411f30; body bytes: 76 */

void __thiscall FUN_00411f30(void *this,int param_1)

{
  if (*(int *)((int)this + 8) != 0) {
    *(undefined4 *)(param_1 + 8) = *(undefined4 *)((int)this + 8);
    *(int *)(*(int *)((int)this + 8) + 4) = param_1;
  }
  *(undefined4 *)(param_1 + 0xc) = *(undefined4 *)((int)this + 0xc);
  *(void **)(param_1 + 4) = this;
  *(int *)((int)this + 8) = param_1;
  return;
}

