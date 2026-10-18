/* Entry: 0x0046cd80; symbol: FUN_0046cd80; body bytes: 124 */

void __thiscall FUN_0046cd80(void *this,undefined4 *param_1,uint param_2)

{
  char *pcVar1;
  uint uVar2;
  int iVar3;
  undefined8 uVar4;
  uint uVar5;
  uint *local_14;
  
  pcVar1 = (char *)FUN_00470620((int)this);
  uVar4 = FUN_00459210(pcVar1,pcVar1 + 0xff,param_2,10);
  uVar5 = 0;
  uVar2 = FUN_0040ff90((int)&stack0x0000000c);
  local_14 = (uint *)uVar4;
  FUN_00470310(&stack0x0000000c,local_14,uVar2,uVar5);
  iVar3 = FUN_0040ff90((int)&stack0x0000000c);
  *(undefined1 *)((int)local_14 + iVar3) = 0;
  FUN_0046ca40(this,param_1);
  return;
}

