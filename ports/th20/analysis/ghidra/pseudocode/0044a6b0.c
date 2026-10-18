/* Entry: 0x0044a6b0; symbol: FUN_0044a6b0; body bytes: 66 */

void __thiscall FUN_0044a6b0(void *this,void *param_1)

{
  FUN_00411ee0(this,(int)param_1);
  FUN_00412dc0(param_1,this);
  if (*(void **)((int)this + 0x14) == this) {
    *(void **)((int)this + 0x14) = param_1;
  }
  return;
}

