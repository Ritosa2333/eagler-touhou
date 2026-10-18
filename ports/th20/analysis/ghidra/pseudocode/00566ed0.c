/* Entry: 0x00566ed0; symbol: FUN_00566ed0; body bytes: 499 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __cdecl
FUN_00566ed0(int param_1,int param_2,int param_3,uint param_4,undefined8 *param_5)

{
  uint uVar1;
  int iVar2;
  float10 fVar3;
  double dVar4;
  double dVar5;
  
  dVar4 = (double)CONCAT44(param_2 & _UNK_00597054,param_1 & _DAT_00597050);
  if (param_3 == 0) {
    dVar5 = DAT_0056c468;
    if (param_4 == 0x7ff00000) {
      if (dVar4 <= DAT_0056c468) {
        if (dVar4 < DAT_0056c468) {
          *param_5 = 0;
          return 0;
        }
        goto LAB_005670a4;
      }
      goto LAB_00566f07;
    }
    if (param_4 == 0xfff00000) {
      if (DAT_0056c468 < dVar4) {
        *param_5 = 0;
        return 0;
      }
      if (dVar4 < DAT_0056c468) {
        *param_5 = DAT_005989c0;
        return 0;
      }
      goto LAB_005670a4;
    }
  }
  if (param_1 != 0) {
    return 0;
  }
  if (param_2 == 0x7ff00000) {
    if (0.0 < (double)CONCAT44(param_4,param_3)) {
LAB_00566f07:
      *param_5 = DAT_005989c0;
      return 0;
    }
    dVar4 = 0.0;
    if (0.0 <= (double)CONCAT44(param_4,param_3)) {
      *param_5 = DAT_0056c468;
      return 0;
    }
    goto LAB_005670b9;
  }
  if (param_2 != -0x100000) {
    return 0;
  }
  uVar1 = FUN_005670d0(param_3,param_4);
  if ((uVar1 & 0x90) == 0) {
    fVar3 = FUN_00564240((double)CONCAT44(param_4,param_3));
    if ((double)fVar3 != (double)CONCAT44(param_4,param_3)) goto LAB_0056705d;
    dVar4 = (double)CONCAT44(param_4,param_3) * _DAT_005a3140;
    fVar3 = FUN_00564240(dVar4);
    if ((double)fVar3 == dVar4) {
      iVar2 = 2;
    }
    else {
      iVar2 = 1;
    }
  }
  else {
LAB_0056705d:
    iVar2 = 0;
  }
  dVar5 = 0.0;
  if ((double)CONCAT44(param_4,param_3) <= 0.0) {
    dVar4 = DAT_0056c468;
    if ((double)CONCAT44(param_4,param_3) < 0.0) {
      if (iVar2 == 1) {
        dVar5 = DAT_005989d0;
      }
LAB_005670a4:
      *param_5 = dVar5;
      return 0;
    }
  }
  else {
    dVar4 = DAT_005989c0;
    if (iVar2 == 1) {
      *param_5 = CONCAT44((uint)((ulonglong)DAT_005989c0 >> 0x20) ^ _UNK_00597074,
                          SUB84(DAT_005989c0,0) ^ _DAT_00597070);
      return 0;
    }
  }
LAB_005670b9:
  *param_5 = dVar4;
  return 0;
}

