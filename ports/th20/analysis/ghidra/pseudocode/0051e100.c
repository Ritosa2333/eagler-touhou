/* Entry: 0x0051e100; symbol: FUN_0051e100; body bytes: 105 */

uint FUN_0051e100(int param_1,float param_2,float param_3)

{
  undefined4 local_8;
  
  local_8 = (uint)((float)param_1 - (((float)param_1 * param_3) / DAT_0056c8cc) * param_2);
  if ((int)local_8 < 0) {
    local_8 = 0;
  }
  else if (0xff < (int)local_8) {
    local_8 = 0xff;
  }
  return local_8 & 0xff;
}

