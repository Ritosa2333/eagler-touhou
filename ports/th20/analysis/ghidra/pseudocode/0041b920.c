/* Entry: 0x0041b920; symbol: FUN_0041b920; body bytes: 417 */

uint * __cdecl FUN_0041b920(uint *param_1,int param_2)

{
  char cVar1;
  
  if (1 < param_2 - (int)param_1 >> 1) {
    cVar1 = FUN_0041bad0(param_1,param_2);
    if (cVar1 == '\0') {
      cVar1 = FUN_00419150((short)*param_1);
      if (cVar1 != '\0') {
        if ((((param_2 - (int)param_1 >> 1 < 4) ||
             (cVar1 = FUN_00419150(*(short *)((int)param_1 + 6)), cVar1 == '\0')) ||
            ((param_2 - (int)param_1 >> 1 != 4 &&
             (cVar1 = FUN_00419150((short)param_1[2]), cVar1 != '\0')))) ||
           (((cVar1 = FUN_00419150(*(short *)((int)param_1 + 2)), cVar1 == '\0' ||
             (((short)param_1[1] != 0x3f && ((short)param_1[1] != 0x2e)))) &&
            ((*(short *)((int)param_1 + 2) != 0x3f || ((short)param_1[1] != 0x3f)))))) {
          if (((2 < param_2 - (int)param_1 >> 1) &&
              (cVar1 = FUN_00419150(*(short *)((int)param_1 + 2)), cVar1 != '\0')) &&
             (cVar1 = FUN_00419150((short)param_1[1]), cVar1 == '\0')) {
            param_1 = (uint *)FUN_004188a0();
          }
        }
        else {
          param_1 = (uint *)((int)param_1 + 6);
        }
      }
    }
    else {
      param_1 = param_1 + 1;
    }
  }
  return param_1;
}

