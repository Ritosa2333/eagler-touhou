/* Entry: 0x00458670; symbol: FUN_00458670; body bytes: 1250 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl
FUN_00458670(float param_1,float param_2,float param_3,float param_4,float param_5,float param_6,
            float param_7,undefined4 param_8,undefined4 param_9,float param_10,int param_11)

{
  char cVar1;
  float local_48 [3];
  int local_3c;
  float local_38 [4];
  float fStack_28;
  float fStack_20;
  float fStack_1c;
  float fStack_14;
  float fStack_10;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  cVar1 = FUN_00457300(param_6,param_7,param_1,param_2,param_3,param_4);
  if ((cVar1 == '\0') &&
     (cVar1 = FUN_00457380(param_1,param_2,param_6,param_7,param_8,param_9,param_10,param_11),
     cVar1 == '\0')) {
    FUN_0040bc20(local_38,0xc,4,FUN_00422e10);
    local_38[0] = (float)((uint)param_3 ^ _DAT_0056d7d0) / DAT_0056c8d0;
    local_38[1] = (float)((uint)param_4 ^ _DAT_0056d7d0) / DAT_0056c8d0;
    local_38[3] = (float)((uint)param_3 ^ _DAT_0056d7d0) / DAT_0056c8d0;
    fStack_28 = param_4 / DAT_0056c8d0;
    fStack_20 = param_3 / DAT_0056c8d0;
    fStack_1c = param_4 / DAT_0056c8d0;
    fStack_14 = param_3 / DAT_0056c8d0;
    fStack_10 = (float)((uint)param_4 ^ _DAT_0056d7d0) / DAT_0056c8d0;
    if (param_5 != _DAT_0056e0e8) {
      FUN_004590e0(local_38,local_38,local_38,param_5,4);
    }
    for (local_3c = 0; local_3c < 4; local_3c = local_3c + 1) {
      FUN_00422dd0(local_48,param_1,param_2,0);
      FUN_004296e0(local_38 + local_3c * 3,local_48);
    }
    cVar1 = FUN_00456d50(local_38[0],local_38[1],local_38[3],fStack_28,param_6,param_7,param_8,
                         param_9,param_10,param_11);
    if (((cVar1 == '\0') &&
        (cVar1 = FUN_00456d50(local_38[3],fStack_28,fStack_20,fStack_1c,param_6,param_7,param_8,
                              param_9,param_10,param_11), cVar1 == '\0')) &&
       (cVar1 = FUN_00456d50(fStack_20,fStack_1c,fStack_14,fStack_10,param_6,param_7,param_8,param_9
                             ,param_10,param_11), cVar1 == '\0')) {
      FUN_00456d50(fStack_14,fStack_10,local_38[0],local_38[1],param_6,param_7,param_8,param_9,
                   param_10,param_11);
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

