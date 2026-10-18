/* Entry: 0x00485390; symbol: FUN_00485390; body bytes: 136 */

undefined4 __cdecl FUN_00485390(float *param_1,float param_2,float param_3)

{
  undefined4 local_8;
  
  if ((((*param_1 + param_2 <= DAT_0056fd6c / DAT_0056c8d0) ||
       (DAT_0056cda4 / DAT_0056c8d0 <= *param_1 - param_2)) || (param_1[1] + param_3 <= 0.0)) ||
     (DAT_0056d7c0 <= param_1[1] - param_3)) {
    local_8 = 1;
  }
  else {
    local_8 = 0;
  }
  return local_8;
}

