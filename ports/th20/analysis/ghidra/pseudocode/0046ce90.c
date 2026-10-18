/* Entry: 0x0046ce90; symbol: FUN_0046ce90; body bytes: 104 */

void __thiscall FUN_0046ce90(void *this,undefined4 *param_1)

{
  char *pcVar1;
  int iVar2;
  
  if (*(int *)((int)this + 0x1a1bc) < 0x140) {
    pcVar1 = (char *)FUN_0046b210((void *)((int)this + 0x11bc),*(int *)((int)this + 0x1a1bc));
    iVar2 = FUN_0040c300((undefined4 *)&stack0x00000008);
    FUN_00548610(pcVar1,0x100,iVar2);
    FUN_0046ca40(this,param_1);
  }
  return;
}

