/* Entry: 0x004f8ff0; symbol: FUN_004f8ff0; body bytes: 466 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __thiscall FUN_004f8ff0(void *this,float *param_1,float param_2,int param_3)

{
  bool bVar1;
  float *pfVar2;
  int iVar3;
  uint uVar4;
  undefined4 uVar5;
  undefined3 extraout_var;
  float10 fVar6;
  float local_14;
  float local_10;
  float local_c;
  
  pfVar2 = (float *)FUN_00460850((int)this);
  fVar6 = FUN_004560d0(pfVar2,param_1);
  if (*(char *)((int)this + 0x204c) == '\0') {
    local_14 = *(float *)((int)this + 0x2094);
  }
  else {
    local_14 = *(float *)((int)this + 0x2098);
  }
  iVar3 = FUN_0041caf0(*(int *)((int)this + 0x14858));
  iVar3 = FUN_004ff490(iVar3);
  local_c = ((float)iVar3 * local_14) / DAT_0056e04c;
  uVar4 = FUN_004ff800((int)this);
  if (uVar4 != 0) {
    local_c = *(float *)((int)this + 0x2238) * _DAT_00573164 * local_c;
  }
  if ((float)fVar6 < local_c * local_c + param_2 * param_2) {
    if ((DAT_005c06a4 != 0) &&
       (bVar1 = FUN_00478160(DAT_005c06a4), CONCAT31(extraout_var,bVar1) != 0)) {
      return 0;
    }
    if (param_3 == 0) {
      iVar3 = FUN_00411700((int)this);
      if (iVar3 == 2) {
        uVar5 = 0;
      }
      else {
        iVar3 = FUN_00411700((int)this);
        if (iVar3 == 4) {
          uVar5 = 0;
        }
        else {
          iVar3 = FUN_00411700((int)this);
          if (iVar3 == 3) {
            uVar5 = 0;
          }
          else {
            bVar1 = FUN_00461070((void *)((int)this + 0x2050),0);
            if (bVar1) {
              uVar5 = 1;
            }
            else {
              FUN_004f86f0();
              uVar5 = 1;
            }
          }
        }
      }
    }
    else {
      uVar5 = 2;
    }
  }
  else {
    local_10 = param_2 / DAT_0056c8d4;
    if (local_10 < DAT_00570560) {
      local_10 = DAT_00570560;
    }
    if ((float)fVar6 < (local_c + local_10) * (local_c + local_10) + param_2 * param_2) {
      uVar5 = 2;
    }
    else {
      uVar5 = 0;
    }
  }
  return uVar5;
}

