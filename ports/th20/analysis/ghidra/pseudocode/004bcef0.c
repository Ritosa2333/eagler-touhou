/* Entry: 0x004bcef0; symbol: FUN_004bcef0; body bytes: 49 */

void __thiscall FUN_004bcef0(void *this,uint param_1,int param_2)

{
  uint uVar1;
  
  uVar1 = *(uint *)((int)this + 0x76b0);
  *(uint *)((int)this + 0x76b0) = uVar1 + param_1;
  *(uint *)((int)this + 0x76b4) =
       *(int *)((int)this + 0x76b4) + param_2 + (uint)CARRY4(uVar1,param_1);
  return;
}

