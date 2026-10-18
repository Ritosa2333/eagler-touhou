/* Entry: 0x004854d0; symbol: FUN_004854d0; body bytes: 176 */

undefined4 __cdecl FUN_004854d0(float *param_1,float param_2,float param_3)

{
  undefined4 local_8;
  
  if ((((param_2 / DAT_0056c8d0 + *param_1 <= DAT_0056fd6c / DAT_0056c8d0) ||
       (DAT_0056cda4 / DAT_0056c8d0 <= *param_1 - param_2 / DAT_0056c8d0)) ||
      (param_3 / DAT_0056c8d0 + param_1[1] <= 0.0)) ||
     (DAT_0056d7c0 <= param_1[1] - param_3 / DAT_0056c8d0)) {
    local_8 = 1;
  }
  else {
    local_8 = 0;
  }
  return local_8;
}

