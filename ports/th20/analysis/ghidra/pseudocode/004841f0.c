/* Entry: 0x004841f0; symbol: FUN_004841f0; body bytes: 103 */

void __thiscall FUN_004841f0(void *this,float param_1)

{
  if ((*(uint *)((int)this + 0xc) & 1) == 0) {
    FUN_00423f50((undefined4 *)this);
    FUN_00423fe0(this,0);
    *(uint *)((int)this + 0xc) = *(uint *)((int)this + 0xc) | 1;
  }
  *(int *)((int)this + 4) = (int)param_1;
  *(float *)((int)this + 8) = param_1;
  *(int *)this = (int)(param_1 - DAT_0056c8cc);
  return;
}

