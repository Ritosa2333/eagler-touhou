/* Entry: 0x00423f80; symbol: FUN_00423f80; body bytes: 90 */

void __thiscall FUN_00423f80(void *this,int param_1)

{
  if ((*(uint *)((int)this + 0xc) & 1) == 0) {
    FUN_00423f50((undefined4 *)this);
    FUN_00423fe0(this,0);
    *(uint *)((int)this + 0xc) = *(uint *)((int)this + 0xc) | 1;
  }
  *(int *)((int)this + 4) = param_1;
  *(float *)((int)this + 8) = (float)param_1;
  *(int *)this = param_1 + -1;
  return;
}

