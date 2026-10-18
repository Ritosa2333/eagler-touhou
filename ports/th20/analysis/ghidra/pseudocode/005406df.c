/* Entry: 0x005406df; symbol: FUN_005406df; body bytes: 115 */

bool FUN_005406df(int *param_1,int *param_2,int param_3)

{
  bool bVar1;
  int local_c;
  int iStack_8;
  
  if (param_3 == 1) {
    bVar1 = (char)*param_1 == (char)*param_2;
  }
  else if (param_3 == 2) {
    bVar1 = (short)*param_1 == (short)*param_2;
  }
  else if (param_3 == 4) {
    bVar1 = *param_1 == *param_2;
  }
  else {
    if (param_3 != 8) {
                    /* WARNING: Subroutine does not return */
      _abort();
    }
    local_c = (int)*(undefined8 *)param_1;
    if ((local_c == *param_2) &&
       (iStack_8 = (int)((ulonglong)*(undefined8 *)param_1 >> 0x20), iStack_8 == param_2[1])) {
      bVar1 = true;
    }
    else {
      bVar1 = false;
    }
  }
  return bVar1;
}

