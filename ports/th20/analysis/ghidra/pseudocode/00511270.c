/* Entry: 0x00511270; symbol: FUN_00511270; body bytes: 140 */

undefined4 __thiscall FUN_00511270(void *this,undefined4 param_1)

{
  int iVar1;
  undefined4 uVar2;
  int *piVar3;
  
  iVar1 = FUN_0044eb30(DAT_005c0028,5,(char **)"fronttr.anm");
  *(int *)((int)this + 0x10) = iVar1;
  if (iVar1 == 0) {
    uVar2 = 0xffffffff;
  }
  else {
    piVar3 = FUN_004122c0(0x2d,FUN_00511300,this);
    *(int **)((int)this + 8) = piVar3;
    piVar3 = FUN_00412360(0x56,FUN_00511310,this);
    *(int **)((int)this + 0xc) = piVar3;
    *(undefined4 *)((int)this + 0x24) = 1;
    FUN_00423520((void *)((int)this + 0x2c),0);
    *(undefined4 *)((int)this + 0x20) = param_1;
    uVar2 = 0;
  }
  return uVar2;
}

