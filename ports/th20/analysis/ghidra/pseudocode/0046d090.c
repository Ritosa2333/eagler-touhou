/* Entry: 0x0046d090; symbol: FUN_0046d090; body bytes: 220 */

void __thiscall FUN_0046d090(void *this,undefined4 *param_1,undefined4 param_2)

{
  undefined4 uVar1;
  undefined4 uVar2;
  char *pcVar3;
  int iVar4;
  
  if (*(int *)((int)this + 0x1a1bc) + 1 < 0x140) {
    pcVar3 = (char *)FUN_0046b210((void *)((int)this + 0x11bc),*(int *)((int)this + 0x1a1bc) + 1);
    iVar4 = FUN_0046b210((void *)((int)this + 0x11bc),*(int *)((int)this + 0x1a1bc));
    FUN_00548610(pcVar3,0x100,iVar4);
    uVar1 = *(undefined4 *)((int)this + 0x1a1c0);
    uVar2 = *(undefined4 *)((int)this + 0x1a1e0);
    *(undefined4 *)((int)this + 0x1a1c0) = *(undefined4 *)((int)this + 0x1a1c4);
    *(undefined4 *)((int)this + 0x1a1e0) = param_2;
    FUN_0046ca40(this,param_1);
    *(undefined4 *)((int)this + 0x1a1c0) = uVar1;
    *(undefined4 *)((int)this + 0x1a1e0) = uVar2;
  }
  return;
}

