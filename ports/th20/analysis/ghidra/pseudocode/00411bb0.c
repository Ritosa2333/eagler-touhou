/* Entry: 0x00411bb0; symbol: FUN_00411bb0; body bytes: 91 */

bool __thiscall FUN_00411bb0(void *this,int *param_1)

{
  bool local_14;
  
  if (param_1 == (int *)0x0) {
                    /* WARNING: Load size is inaccurate */
    local_14 = *this == 0;
  }
  else {
                    /* WARNING: Load size is inaccurate */
    local_14 = *this == *param_1;
  }
  local_14 = !local_14;
  return local_14;
}

