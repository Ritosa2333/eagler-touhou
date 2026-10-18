/* Entry: 0x0045add0; symbol: FUN_0045add0; body bytes: 319 */

void __thiscall FUN_0045add0(void *this,undefined4 param_1,undefined4 param_2,uint param_3)

{
  int *piVar1;
  int iVar2;
  float10 fVar3;
  int local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  if (((*(int *)((int)this + 8) != 0) &&
      (piVar1 = (int *)FUN_0045a4a0((int)this), piVar1 != (int *)0x0)) &&
     (iVar2 = FUN_0045b780(this,piVar1,&local_c), -1 < iVar2)) {
    if (local_c != 0) {
      iVar2 = FUN_0045a240(this,piVar1,0,param_3);
      if (iVar2 < 0) goto LAB_0045aeff;
      FUN_0045b410((int)this);
    }
    *(undefined4 *)((int)this + 0x20) = 0;
    *(undefined4 *)((int)this + 0x18) = 0;
    *(undefined4 *)((int)this + 0x1c) = 0;
    FUN_0045b9b0(this,0);
    *(undefined4 *)((int)this + 0x58) = 1;
    *(undefined4 *)((int)this + 0x24) = param_1;
    *(undefined4 *)((int)this + 0x28) = param_2;
    *(undefined4 *)((int)this + 0x30) = 0;
    fVar3 = (float10)FUN_0041cb10();
    *(double *)((int)this + 0x38) = (double)fVar3;
    *(undefined8 *)((int)this + 0x50) = 0;
    *(undefined8 *)((int)this + 0x48) = 0;
    *(undefined8 *)((int)this + 0x40) = 0;
    (**(code **)(*piVar1 + 0x30))(piVar1,0,param_1,param_2);
  }
LAB_0045aeff:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

