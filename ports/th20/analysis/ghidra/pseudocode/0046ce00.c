/* Entry: 0x0046ce00; symbol: FUN_0046ce00; body bytes: 135 */

void __thiscall
FUN_0046ce00(void *this,undefined4 *param_1,float param_2,undefined4 param_3,undefined4 param_4,
            size_t param_5)

{
  uint *puVar1;
  uint uVar2;
  int iVar3;
  undefined8 uVar4;
  uint uVar5;
  uint *local_14;
  
  puVar1 = (uint *)FUN_00470620((int)this);
  uVar4 = FUN_004710f0(puVar1,(uint *)((int)puVar1 + 0xff),param_2,2,param_5);
  uVar5 = 0;
  uVar2 = FUN_0040ff90((int)&param_3);
  local_14 = (uint *)uVar4;
  FUN_00470310(&param_3,local_14,uVar2,uVar5);
  iVar3 = FUN_0040ff90((int)&param_3);
  *(undefined1 *)((int)local_14 + iVar3) = 0;
  FUN_0046ca40(this,param_1);
  return;
}

