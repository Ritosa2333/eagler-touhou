/* Entry: 0x00506480; symbol: FUN_00506480; body bytes: 46 */

int __thiscall FUN_00506480(void *this,uint param_1)

{
  return (param_1 & 0xff) * 0x78 +
         *(int *)(*(int *)((int)this + 0x2208) + 0x5d4 + ((int)param_1 >> 8) * 4);
}

