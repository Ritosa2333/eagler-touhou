/* Entry: 0x004c9300; symbol: FUN_004c9300; body bytes: 75 */

void __thiscall FUN_004c9300(void *this,int param_1)

{
  FUN_0044a670((void *)(param_1 + 8),param_1);
  FUN_0044a6b0((void *)((int)this + 0x10),(void *)(param_1 + 8));
  *(int *)((int)this + 0x28) = *(int *)((int)this + 0x28) + 1;
  return;
}

