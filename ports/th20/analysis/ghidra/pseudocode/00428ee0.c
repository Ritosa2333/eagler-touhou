/* Entry: 0x00428ee0; symbol: FUN_00428ee0; body bytes: 119 */

void __thiscall FUN_00428ee0(void *this,int param_1)

{
  int iVar1;
  
  iVar1 = FUN_00426110((void *)((int)this + 0x187c),param_1);
  if (*(int *)(iVar1 + 4) != 0) {
    iVar1 = FUN_00426110((void *)((int)this + 0x187c),param_1);
    FUN_0041f670(*(LPVOID *)(iVar1 + 4));
    iVar1 = FUN_00426110((void *)((int)this + 0x187c),param_1);
    *(undefined4 *)(iVar1 + 4) = 0;
  }
  return;
}

