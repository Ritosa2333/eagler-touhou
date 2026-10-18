/* Entry: 0x004f5110; symbol: FUN_004f5110; body bytes: 48 */

int __thiscall FUN_004f5110(void *this,int param_1)

{
  *(undefined4 *)((int)this + 4) = *(undefined4 *)(param_1 + 4);
  (**(code **)(**(int **)((int)this + 4) + 4))();
  return (int)this;
}

