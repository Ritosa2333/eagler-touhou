/* Entry: 0x004f1e70; symbol: FUN_004f1e70; body bytes: 393 */

/* WARNING: Type propagation algorithm not settling */

void * __cdecl
FUN_004f1e70(void *param_1,uint *param_2,uint *param_3,undefined4 param_4,int param_5)

{
  undefined8 uVar1;
  uint local_38;
  size_t local_34;
  int local_2c [4];
  undefined4 local_1c;
  undefined4 local_18;
  uint local_14;
  uint local_10;
  uint local_c;
  uint local_8;
  
  local_8 = FUN_004fe160();
  if (local_8 == 0) {
    if (param_5 == 1) {
      if ((int)param_3 - (int)param_2 < 5) {
        local_18 = 0x84;
        FUN_00467cc0(param_1,&param_3,&local_18);
      }
      else {
        FUN_00543e20(param_2,(uint *)"0e+00",5);
        local_2c[3] = (int)param_2 + 5;
        local_1c = 0;
        FUN_00467cc0(param_1,local_2c + 3,&local_1c);
      }
    }
    else if (param_2 == param_3) {
      local_2c[2] = 0x84;
      FUN_00467cc0(param_1,&param_3,local_2c + 2);
    }
    else {
      *(undefined1 *)param_2 = 0x30;
      local_2c[0] = (int)param_2 + 1;
      local_2c[1] = 0;
      FUN_00467cc0(param_1,local_2c,local_2c + 1);
    }
  }
  else {
    local_10 = local_8 & 0x7fffff;
    local_c = local_8 >> 0x17;
    if (param_5 == 2) {
      local_14 = local_c - 0x96;
      if (0 < (int)local_14) {
        FUN_004ed320(param_1,(char *)param_2,(int)param_3,local_10 | 0x800000,local_14);
        return param_1;
      }
    }
    uVar1 = FUN_004fdb80(local_10,local_c);
    local_34 = (size_t)((ulonglong)uVar1 >> 0x20);
    local_38 = (uint)uVar1;
    FUN_004f2000(param_1,param_2,(int)param_3,local_38,local_34,param_5,local_10,local_c);
  }
  return param_1;
}

