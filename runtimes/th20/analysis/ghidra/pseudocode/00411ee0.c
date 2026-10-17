/* Entry: 0x00411ee0; symbol: FUN_00411ee0; body bytes: 76 */

void __thiscall FUN_00411ee0(void *this,int param_1)

{
  if (*(int *)((int)this + 4) != 0) {
    *(undefined4 *)(param_1 + 4) = *(undefined4 *)((int)this + 4);
    *(int *)(*(int *)((int)this + 4) + 8) = param_1;
  }
  *(int *)((int)this + 4) = param_1;
  *(undefined4 *)(param_1 + 0xc) = *(undefined4 *)((int)this + 0xc);
  *(void **)(param_1 + 8) = this;
  return;
}

