/* Entry: 0x00489a10; symbol: FUN_00489a10; body bytes: 94 */

void __thiscall FUN_00489a10(void *this,undefined4 param_1)

{
  undefined4 uVar1;
  undefined4 uVar2;
  
  uVar1 = move<>(param_1);
  uVar2 = move<>(*(undefined4 *)((int)this + 4));
  FUN_0048ae30(*(undefined4 *)((int)this + 8),uVar2,uVar1);
  *(int *)((int)this + 4) = *(int *)((int)this + 4) + 0x184;
  return;
}

