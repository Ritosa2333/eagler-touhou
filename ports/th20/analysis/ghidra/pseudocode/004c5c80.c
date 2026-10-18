/* Entry: 0x004c5c80; symbol: FUN_004c5c80; body bytes: 63 */

int __thiscall FUN_004c5c80(void *this,uint param_1)

{
  uint uVar1;
  
  uVar1 = FUN_004b6d80(this,param_1);
  return *(int *)(*(int *)((int)this + 4) + uVar1 * 4) + (param_1 % 4) * 4;
}

