/* Entry: 0x004ab780; symbol: FUN_004ab780; body bytes: 154 */

void __thiscall FUN_004ab780(void *this,uint param_1,int param_2)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  
  uVar1 = FUN_004aba00((int)this + 0x340);
  if (uVar1 < param_1 + 1) {
    FUN_004ab570((void *)((int)this + 0x340),param_1 + 1);
  }
  if (param_2 == 0) {
    iVar2 = FUN_0048bd40((void *)((int)this + 0x340),param_1);
    *(undefined1 *)(iVar2 + 0x48) = 0;
  }
  else {
    iVar3 = 0x40;
    iVar2 = FUN_004aa160((void *)((int)this + 0x340),param_1);
    FUN_00548610((char *)(iVar2 + 0x48),iVar3,param_2);
  }
  return;
}

