/* Entry: 0x00485740; symbol: FUN_00485740; body bytes: 434 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

float10 __cdecl FUN_00485740(int param_1,float param_2,float *param_3,float param_4,float param_5)

{
  int iVar1;
  void *this;
  float *pfVar2;
  float10 fVar3;
  float10 fVar4;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  undefined4 local_8;
  
  if (param_5 <= DAT_0056fd80) {
    param_5 = DAT_0056e0f0;
  }
  if (DAT_0056fd80 < param_4) {
    if ((param_4 < _DAT_0056fd54) || (DAT_0056fd58 <= param_4)) {
      if ((param_4 < _DAT_0056fd5c) || (DAT_0056fd60 <= param_4)) {
        if ((param_4 < DAT_0056fd60) || (DAT_0056fd64 <= param_4)) {
          if (param_4 < DAT_0056fd64) {
            local_8 = param_4;
          }
          else {
            this = (void *)FUN_00478060(param_1);
            iVar1 = FUN_00485660(this,0);
            pfVar2 = (float *)FUN_0047a2c0(iVar1);
            fVar3 = (float10)FUN_0047a2e0(param_3,pfVar2);
            local_8 = (float)fVar3;
          }
          local_c = local_8;
        }
        else {
          fVar3 = FUN_004298e0(0x5ba4a8);
          local_c = (float)fVar3 * param_5 + param_2;
        }
        local_10 = local_c;
      }
      else {
        iVar1 = FUN_00460830(param_1);
        fVar3 = FUN_004ff350(iVar1,param_3);
        fVar4 = FUN_004298e0(0x5ba4a8);
        local_10 = (float)fVar3 + (float)fVar4 * param_5;
      }
      local_14 = local_10;
    }
    else {
      iVar1 = FUN_00460830(param_1);
      fVar3 = FUN_004ff350(iVar1,param_3);
      local_14 = (float)fVar3 + param_5;
    }
    local_18 = local_14;
  }
  else {
    local_18 = param_2;
  }
  return (float10)local_18;
}

