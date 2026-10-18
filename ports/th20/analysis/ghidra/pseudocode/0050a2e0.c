/* Entry: 0x0050a2e0; symbol: FUN_0050a2e0; body bytes: 94 */

int __thiscall FUN_0050a2e0(void *this,int param_1)

{
  void *pvVar1;
  
  pvVar1 = FUN_00507190();
  FUN_00508b60((void *)((int)this + param_1 * 0x14 + 0x40),(int)pvVar1 + 0x3d868);
  *(int *)((int)this + 0xe4) = *(int *)((int)this + 0xe4) + 1;
  return (int)pvVar1 + 0x3d868;
}

