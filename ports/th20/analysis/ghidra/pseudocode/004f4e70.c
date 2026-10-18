/* Entry: 0x004f4e70; symbol: FUN_004f4e70; body bytes: 69 */

uint * __thiscall FUN_004f4e70(void *this,uint param_1)

{
                    /* WARNING: Load size is inaccurate */
  *(uint *)this = *this & 0xf0000000 | param_1 & 0xfffffff;
                    /* WARNING: Load size is inaccurate */
  *(uint *)this = *this & 0xfffffff;
  FUN_004fcdf0(this,0);
  return (uint *)this;
}

