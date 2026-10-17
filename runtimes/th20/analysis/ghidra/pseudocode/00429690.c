/* Entry: 0x00429690; symbol: FUN_00429690; body bytes: 77 */

float * __thiscall FUN_00429690(void *this,float param_1)

{
                    /* WARNING: Load size is inaccurate */
  *(float *)this = *this * param_1;
  *(float *)((int)this + 4) = *(float *)((int)this + 4) * param_1;
  *(float *)((int)this + 8) = *(float *)((int)this + 8) * param_1;
  return (float *)this;
}

