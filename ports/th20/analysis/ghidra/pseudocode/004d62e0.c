/* Entry: 0x004d62e0; symbol: FUN_004d62e0; body bytes: 172 */

void __thiscall
FUN_004d62e0(void *this,undefined4 *param_1,float *param_2,float *param_3,void *param_4,
            float param_5,float param_6,float param_7,int param_8)

{
  undefined4 *local_8;
  
  local_8 = (undefined4 *)this;
  while( true ) {
    if (local_8 == (undefined4 *)0x0) {
      return;
    }
    if (((float)local_8[2] <= param_7) && (param_7 < (float)local_8[3])) break;
    local_8 = (undefined4 *)*local_8;
  }
  if (param_8 == 0) {
    FUN_004d6390(local_8,param_1,param_2,param_3,param_7);
    return;
  }
  FUN_004d6830(local_8,param_1,param_2,param_3,param_4,param_5,param_6,param_7);
  return;
}

