/* Entry: 0x00456920; symbol: FUN_00456920; body bytes: 533 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined1 __cdecl
FUN_00456920(float param_1,float param_2,float param_3,float param_4,float param_5,float param_6,
            float param_7,float param_8)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  undefined1 uVar5;
  float fVar6;
  float fVar7;
  
  fVar4 = param_8;
  fVar3 = param_7;
  fVar2 = param_4;
  fVar1 = param_3;
  fVar6 = (param_1 - param_3) * (param_6 - param_2) + (param_2 - param_4) * (param_1 - param_5);
  fVar7 = (param_1 - param_3) * (param_8 - param_2) + (param_2 - param_4) * (param_1 - param_7);
  if (fVar6 * fVar7 <= _DAT_0056e0e8) {
    if ((fVar6 == _DAT_0056e0e8) && (fVar7 == _DAT_0056e0e8)) {
      if (param_3 < param_1) {
        param_3 = param_1;
        param_1 = fVar1;
        param_4 = param_2;
        param_2 = fVar2;
      }
      if (param_7 < param_5) {
        param_7 = param_5;
        param_5 = fVar3;
        param_8 = param_6;
        param_6 = fVar4;
      }
      if ((((param_7 < param_1) || (param_8 < param_2)) || (param_3 < param_5)) ||
         (param_4 < param_6)) {
        uVar5 = 0;
      }
      else {
        uVar5 = 1;
      }
    }
    else if (((param_5 - param_7) * (param_2 - param_6) + (param_6 - param_8) * (param_5 - param_1))
             * ((param_5 - param_7) * (param_4 - param_6) +
               (param_6 - param_8) * (param_5 - param_3)) <= _DAT_0056e0e8) {
      uVar5 = 1;
    }
    else {
      uVar5 = 0;
    }
  }
  else {
    uVar5 = 0;
  }
  return uVar5;
}

