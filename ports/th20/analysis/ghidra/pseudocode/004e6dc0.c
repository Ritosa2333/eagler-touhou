/* Entry: 0x004e6dc0; symbol: FUN_004e6dc0; body bytes: 75 */

undefined1 FUN_004e6dc0(uint *param_1,uint *param_2)

{
  undefined1 local_10;
  
  if ((param_2[1] < param_1[1]) || ((param_2[1] <= param_1[1] && (*param_2 <= *param_1)))) {
    local_10 = 0;
  }
  else {
    local_10 = 1;
  }
  return local_10;
}

