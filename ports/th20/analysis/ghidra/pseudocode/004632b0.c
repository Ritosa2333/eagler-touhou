/* Entry: 0x004632b0; symbol: FUN_004632b0; body bytes: 82 */

void __thiscall FUN_004632b0(void *this,byte *param_1)

{
  *(char *)((int)this + 2) = (char)((uint)*(byte *)((int)this + 2) + (uint)param_1[2] >> 1);
  *(char *)((int)this + 1) = (char)((uint)*(byte *)((int)this + 1) + (uint)param_1[1] >> 1);
                    /* WARNING: Load size is inaccurate */
  *(char *)this = (char)((uint)*this + (uint)*param_1 >> 1);
  return;
}

