/* Entry: 0x00429180; symbol: FUN_00429180; body bytes: 82 */

uint * __thiscall FUN_00429180(void *this,byte *param_1)

{
  *(undefined4 *)this = 0;
  *(undefined4 *)((int)this + 4) = 0;
  *(undefined4 *)((int)this + 8) = 0;
  *(uint *)((int)this + 8) = (uint)param_1[2];
  *(uint *)((int)this + 4) = (uint)param_1[1];
  *(uint *)this = (uint)*param_1;
  return (uint *)this;
}

