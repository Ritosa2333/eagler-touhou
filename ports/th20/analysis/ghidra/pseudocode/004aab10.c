/* Entry: 0x004aab10; symbol: FUN_004aab10; body bytes: 89 */

void __thiscall FUN_004aab10(void *this,int param_1)

{
  int iVar1;
  float10 fVar2;
  
  iVar1 = *(int *)((int)this + 0x4c);
  fVar2 = FUN_00423bd0((int)this + 0x3c);
  FUN_00454ef0(*(void **)((int)this + param_1 * 4 + 0x50),(float)fVar2,(float)iVar1);
  return;
}

