/* Entry: 0x00485420; symbol: FUN_00485420; body bytes: 176 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __cdecl
FUN_00485420(float *param_1,float param_2,float param_3,float param_4,float param_5)

{
  undefined4 local_8;
  
  if ((((*param_1 + param_2 <= (float)((uint)param_4 ^ _DAT_0056d7d0) / DAT_0056c8d0) ||
       (param_4 / DAT_0056c8d0 <= *param_1 - param_2)) ||
      (param_1[1] + param_3 <= DAT_0056ed14 - param_5 / DAT_0056c8d0)) ||
     (param_5 / DAT_0056c8d0 + DAT_0056ed14 <= param_1[1] - param_3)) {
    local_8 = 1;
  }
  else {
    local_8 = 0;
  }
  return local_8;
}

