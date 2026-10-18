/* Entry: 0x0046cfa0; symbol: FUN_0046cfa0; body bytes: 124 */

void __thiscall
FUN_0046cfa0(void *this,undefined4 *param_1,undefined4 param_2,undefined4 param_3,uint param_4)

{
  uint *puVar1;
  uint uVar2;
  char *pcVar3;
  int iVar4;
  undefined8 uVar5;
  uint uVar6;
  undefined1 *local_14;
  
  puVar1 = (uint *)FUN_00470620((int)this);
  uVar6 = 0;
  uVar2 = FUN_0040ff90((int)&param_2);
  FUN_00470310(&param_2,puVar1,uVar2,uVar6);
  uVar6 = 10;
  pcVar3 = (char *)((int)puVar1 + 0xff);
  uVar2 = param_4;
  iVar4 = FUN_0040ff90((int)&param_2);
  uVar5 = FUN_00459210((char *)(iVar4 + (int)puVar1),pcVar3,uVar2,uVar6);
  local_14 = (undefined1 *)uVar5;
  *local_14 = 0;
  FUN_0046ca40(this,param_1);
  return;
}

