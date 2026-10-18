/* Entry: 0x004c1eb0; symbol: FUN_004c1eb0; body bytes: 70 */

void __thiscall FUN_004c1eb0(void *this,void *param_1)

{
  bool bVar1;
  
  if (param_1 != (void *)0x0) {
    FUN_00411ce0((int)param_1);
    bVar1 = FUN_004c1000((int)param_1);
    if (!bVar1) {
      FUN_0044a6b0((void *)((int)this + 0xc42c),param_1);
    }
  }
  return;
}

