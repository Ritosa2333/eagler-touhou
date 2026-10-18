/* Entry: 0x00411420; symbol: FUN_00411420; body bytes: 81 */

undefined4 __thiscall FUN_00411420(void *this,undefined4 param_1)

{
  int *piVar1;
  undefined4 uVar2;
  code *pcVar3;
  undefined4 uVar4;
  
  uVar4 = 0;
  piVar1 = (int *)FUN_0040ff90((int)this);
  pcVar3 = *(code **)(*piVar1 + 8);
  uVar2 = FUN_0040c300((undefined4 *)this);
  (*pcVar3)(param_1,uVar2,pcVar3,uVar2,uVar4);
  return param_1;
}

