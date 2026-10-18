/* Entry: 0x004262f0; symbol: FUN_004262f0; body bytes: 63 */

void __thiscall FUN_004262f0(void *this,float param_1)

{
  float fVar1;
  
  *(undefined4 *)((int)this + 0x20) = 1;
  fVar1 = param_1 * DAT_0056e048;
  *(int *)((int)this + 0x18) = (int)fVar1;
  *(int *)((int)this + 0x1c) = (int)fVar1;
  return;
}

