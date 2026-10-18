/* Entry: 0x004bcf30; symbol: FUN_004bcf30; body bytes: 46 */

void __thiscall FUN_004bcf30(void *this,uint param_1,int param_2)

{
  uint uVar1;
  
  uVar1 = *(uint *)((int)this + 0x60);
  *(uint *)((int)this + 0x60) = uVar1 + param_1;
  *(uint *)((int)this + 100) = *(int *)((int)this + 100) + param_2 + (uint)CARRY4(uVar1,param_1);
  FUN_004beb60((int)this);
  return;
}

