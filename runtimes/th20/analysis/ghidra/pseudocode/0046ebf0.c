/* Entry: 0x0046ebf0; symbol: FUN_0046ebf0; body bytes: 85 */

void __thiscall FUN_0046ebf0(void *this,int param_1)

{
  uint uVar1;
  uint uVar2;
  uint *puVar3;
  uint *puVar4;
  
  uVar1 = *(uint *)(param_1 + 0x14);
  puVar3 = Myptr((undefined4 *)(param_1 + 4));
  uVar2 = *(uint *)((int)this + 0x14);
  puVar4 = Myptr((undefined4 *)((int)this + 4));
  FUN_00447200(puVar4,uVar2,puVar3,uVar1);
  return;
}

