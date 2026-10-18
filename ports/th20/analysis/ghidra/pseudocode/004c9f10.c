/* Entry: 0x004c9f10; symbol: FUN_004c9f10; body bytes: 1131 */

void __thiscall
FUN_004c9f10(void *this,float *param_1,float *param_2,float param_3,undefined4 param_4,int param_5)

{
  char cVar1;
  float *pfVar2;
  int iVar3;
  void *this_00;
  undefined1 local_26c [12];
  undefined4 local_260;
  undefined4 local_25c;
  undefined4 local_258;
  code *local_254;
  undefined4 local_250;
  int local_24c;
  undefined4 local_248;
  int local_244;
  undefined4 local_240;
  int local_23c;
  int *local_238;
  int *local_234;
  float local_230;
  int local_22c;
  int local_228;
  int *local_224;
  float local_220;
  float local_21c;
  undefined4 local_218;
  float local_214;
  float local_210;
  undefined4 local_20c;
  undefined1 local_208 [512];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_224 = (int *)this;
  if ((param_5 == 0) || (*(int *)((int)this + 0x6cc) == 0)) {
    local_22c = 0;
    local_228 = 0;
    FUN_00422e10(&local_214);
    FUN_00422e10(&local_220);
    local_230 = DAT_0056cd90 / DAT_0056c8d0;
    _memset(local_208,0,0x200);
    FUN_00439330(this_00,&local_220,(float)local_224[0x1c],DAT_0056cd90 / DAT_0056c8d0);
    local_218 = 0;
    local_238 = local_224 + 0x16;
    pfVar2 = (float *)FUN_00429570(local_238,local_26c,&local_220);
    local_214 = *pfVar2;
    local_210 = pfVar2[1];
    local_20c = 0;
    FUN_004296e0(&local_220,&local_220);
    for (; DAT_0056cd90 / DAT_0056c8d0 + local_230 <= (float)local_224[0x1d];
        local_230 = local_230 + DAT_0056cd90) {
      cVar1 = FUN_004580c0(*param_1,param_1[1],*param_2,param_2[1],param_3,local_214,local_210,
                           local_220,local_21c,(float)local_224[0x1c]);
      if (cVar1 != '\0') {
        local_208[local_228] = 1;
        local_22c = local_22c + 1;
        iVar3 = FUN_0040c300((undefined4 *)local_224[0x1bc]);
        local_234 = (int *)(iVar3 + 0x286d88);
        *local_234 = *local_234 + 1;
        if (((local_224[0x1b9] < 0x12) || (local_224[0x1b9] == 0x22)) || (local_224[0x1b9] == 0x26))
        {
          iVar3 = FUN_0040c300((undefined4 *)local_224[0x1bc]);
          local_240 = FUN_004859b0(iVar3);
          local_23c = local_224[0x1d4] * 2 + 0xd4;
          FUN_004790e0(&local_258,(uint *)"bullet",local_23c,&local_214,0,-1,(undefined4 *)0x0);
        }
        else if ((local_224[0x1b9] < 0x20) || (local_224[0x1b9] == 0x1b)) {
          iVar3 = FUN_0040c300((undefined4 *)local_224[0x1bc]);
          local_248 = FUN_004859b0(iVar3);
          local_244 = local_224[0x1d4] * 2 + 0x104;
          FUN_004790e0(&local_25c,(uint *)"bullet",local_244,&local_214,0,-1,(undefined4 *)0x0);
        }
        else if (local_224[0x1b9] < 0x22) {
          iVar3 = FUN_0040c300((undefined4 *)local_224[0x1bc]);
          local_250 = FUN_004859b0(iVar3);
          local_24c = local_224[0x1d4] * 2 + 0x11c;
          FUN_004790e0(&local_260,(uint *)"bullet",local_24c,&local_214,0,-1,(undefined4 *)0x0);
        }
      }
      FUN_004296e0(&local_214,&local_220);
      local_228 = local_228 + 1;
    }
    if (local_22c != 0) {
      local_254 = *(code **)(*local_224 + 0x38);
      (*local_254)(local_208,local_228);
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

