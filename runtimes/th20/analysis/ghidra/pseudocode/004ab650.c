/* Entry: 0x004ab650; symbol: FUN_004ab650; body bytes: 301 */

void __thiscall FUN_004ab650(void *this,int param_1,int param_2,undefined4 param_3,int param_4)

{
  uint uVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  
  uVar1 = FUN_004aba00((int)this + 0x340);
  if (uVar1 < param_1 + 1U) {
    FUN_004ab570((void *)((int)this + 0x340),param_1 + 1);
  }
  piVar2 = (int *)FUN_0048bd40((void *)((int)this + 0x340),param_1);
  *piVar2 = param_2;
  if (-1 < param_2) {
    iVar3 = FUN_0048bd40((void *)((int)this + 0x340),param_1);
    *(undefined4 *)(iVar3 + 4) = param_3;
    if (param_4 == 0) {
      iVar3 = FUN_0048bd40((void *)((int)this + 0x340),param_1);
      *(undefined1 *)(iVar3 + 8) = 0;
      iVar3 = FUN_0048bd40((void *)((int)this + 0x340),param_1);
      *(undefined1 *)(iVar3 + 0x48) = 0;
    }
    else {
      iVar5 = 0x40;
      iVar3 = param_4;
      iVar4 = FUN_0048bd40((void *)((int)this + 0x340),param_1);
      FUN_00548610((char *)(iVar4 + 8),iVar5,iVar3);
      iVar4 = 0x40;
      iVar3 = FUN_0048bd40((void *)((int)this + 0x340),param_1);
      FUN_00548610((char *)(iVar3 + 0x48),iVar4,param_4);
    }
  }
  return;
}

