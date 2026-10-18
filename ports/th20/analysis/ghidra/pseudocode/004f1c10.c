/* Entry: 0x004f1c10; symbol: FUN_004f1c10; body bytes: 603 */

/* WARNING: Type propagation algorithm not settling */

void __cdecl FUN_004f1c10(void *param_1,uint *param_2,uint *param_3,undefined8 param_4,int param_5)

{
  undefined4 uVar1;
  uint *puVar2;
  longlong lVar3;
  ulonglong uVar4;
  longlong local_78 [2];
  uint local_68;
  uint local_64;
  uint local_60;
  uint local_5c;
  int local_58;
  int local_54;
  int local_50 [4];
  undefined4 local_40;
  undefined4 local_3c;
  undefined8 local_38;
  uint local_30;
  uint local_2c;
  undefined8 local_28;
  uint local_20;
  char local_19;
  uint local_18;
  uint local_14;
  size_t local_10;
  uint local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  lVar3 = FUN_0046fc30();
  local_28._4_4_ = (uint)((ulonglong)lVar3 >> 0x20);
  local_28._0_4_ = (uint)lVar3;
  if (lVar3 == 0) {
    local_28 = lVar3;
    if (param_5 == 1) {
      if ((int)param_3 - (int)param_2 < 5) {
        local_3c = 0x84;
        FUN_00467cc0(param_1,&param_3,&local_3c);
      }
      else {
        FUN_00543e20(param_2,(uint *)"0e+00",5);
        local_50[3] = (int)param_2 + 5;
        local_40 = 0;
        FUN_00467cc0(param_1,local_50 + 3,&local_40);
      }
    }
    else if (param_2 == param_3) {
      local_50[2] = 0x84;
      FUN_00467cc0(param_1,&param_3,local_50 + 2);
    }
    else {
      *(undefined1 *)param_2 = 0x30;
      local_50[0] = (int)param_2 + 1;
      local_50[1] = 0;
      FUN_00467cc0(param_1,local_50,local_50 + 1);
    }
  }
  else {
    local_2c = local_28._4_4_ & 0xfffff;
    local_30 = (uint)local_28;
    uVar4 = __aullshr(0x34,local_28._4_4_);
    local_20 = (uint)uVar4;
    if ((param_5 == 2) && (local_54 = local_20 - 0x433, 0 < local_54)) {
      FUN_00469720(param_1,(char *)param_2,(char *)param_3,param_4,0);
    }
    else {
      uVar1 = FUN_004fd8b0(local_30,local_2c,local_20,(ulonglong *)&local_18);
      local_19 = (char)uVar1;
      if (local_19 == '\0') {
        puVar2 = (uint *)FUN_004fd000(local_78,local_30,local_2c,local_20);
        local_68 = *puVar2;
        local_64 = puVar2[1];
        local_60 = puVar2[2];
        local_5c = puVar2[3];
        uVar4 = local_38;
        local_18 = local_68;
        local_14 = local_64;
        local_10 = local_60;
        local_c = local_5c;
      }
      else {
        while( true ) {
          uVar4 = FUN_004fdb20(local_18,local_14);
          local_38._0_4_ = (uint)uVar4;
          local_58 = local_18 + (uint)local_38 * -10;
          if (local_58 != 0) break;
          local_18 = (uint)local_38;
          local_38._4_4_ = (uint)(uVar4 >> 0x20);
          local_14 = local_38._4_4_;
          local_10 = local_10 + 1;
          local_58 = 0;
          local_38 = uVar4;
        }
      }
      local_38 = uVar4;
      FUN_004f2630(param_1,param_2,(char *)param_3,local_18,local_14,local_10,local_c,param_5,
                   param_4);
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

