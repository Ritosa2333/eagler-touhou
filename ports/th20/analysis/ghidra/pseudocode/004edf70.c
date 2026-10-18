/* Entry: 0x004edf70; symbol: FUN_004edf70; body bytes: 448 */

byte * __cdecl FUN_004edf70(byte *param_1,byte *param_2,int *param_3)

{
  byte bVar1;
  
  if (((param_1 != param_2) && (*param_1 != 0x7d)) &&
     (param_1 = FUN_004edbd0(param_1,param_2,param_3), param_1 != param_2)) {
    bVar1 = *param_1;
    if (bVar1 == 0x20) {
      FUN_004fcaf0(param_3,3);
      param_1 = param_1 + 1;
    }
    else if (bVar1 == 0x2b) {
      FUN_004fcaf0(param_3,1);
      param_1 = param_1 + 1;
    }
    else if (bVar1 == 0x2d) {
      FUN_004fcaf0(param_3,2);
      param_1 = param_1 + 1;
    }
    if (param_1 != param_2) {
      if (*param_1 == 0x23) {
        FUN_004fc800(param_3);
        param_1 = param_1 + 1;
        if (param_1 == param_2) {
          return param_1;
        }
      }
      if (*param_1 == 0x30) {
        FUN_004fcbe0(param_3);
        param_1 = param_1 + 1;
        if (param_1 == param_2) {
          return param_1;
        }
      }
      param_1 = (byte *)FUN_004ee530((char *)param_1,(char *)param_2,param_3);
      if ((param_1 != param_2) &&
         ((*param_1 != 0x2e ||
          (param_1 = (byte *)FUN_004ee310((char *)param_1,(char *)param_2,param_3),
          param_1 != param_2)))) {
        if (*param_1 == 0x4c) {
          FUN_004fc850(param_3);
          param_1 = param_1 + 1;
          if (param_1 == param_2) {
            return param_1;
          }
        }
        if (*param_1 == 0x7d) {
          FUN_004ed910(param_3,'\0');
        }
        else {
          FUN_004ed910(param_3,*param_1);
          param_1 = param_1 + 1;
        }
      }
    }
  }
  return param_1;
}

