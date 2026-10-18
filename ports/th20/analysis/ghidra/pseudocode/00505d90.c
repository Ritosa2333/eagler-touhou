/* Entry: 0x00505d90; symbol: FUN_00505d90; body bytes: 174 */

undefined4 __thiscall
FUN_00505d90(void *this,char *param_1,undefined4 param_2,undefined4 *param_3,char *param_4)

{
  void *this_00;
  int iVar1;
  int iVar2;
  undefined4 *this_01;
  undefined4 uVar3;
  
  this_00 = (void *)FUN_0040ff90(*(int *)((int)this + 0x12594));
  iVar1 = FUN_00506480(this_00,(uint)param_1);
  if ((*(char *)(iVar1 + 0x29) == '\x02') &&
     (iVar2 = FUN_00506190(this,*(int *)(iVar1 + 0x38)), iVar2 != 0)) {
    iVar1 = FUN_00506160(this,*(int *)(iVar1 + 0x38));
    *(char **)(iVar1 + 0xd8) = param_1;
    return 0;
  }
  this_01 = FUN_00506210((int)this);
  if (this_01 == (undefined4 *)0x0) {
    uVar3 = 0;
  }
  else {
    iVar1 = FUN_00504e90(this_01,param_1,param_2,param_3,param_4,
                         *(undefined4 *)((int)this + 0x12590));
    if (iVar1 == 0) {
      uVar3 = 0;
    }
    else {
      uVar3 = 0xffffffff;
    }
  }
  return uVar3;
}

