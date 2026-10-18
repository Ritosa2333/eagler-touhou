/* Entry: 0x004fcdf0; symbol: FUN_004fcdf0; body bytes: 41 */

void __thiscall FUN_004fcdf0(void *this,byte param_1)

{
                    /* WARNING: Load size is inaccurate */
  *(uint *)this = *this & 0xfffffff | (uint)param_1 << 0x1c;
  return;
}

