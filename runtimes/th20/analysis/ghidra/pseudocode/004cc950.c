/* Entry: 0x004cc950; symbol: FUN_004cc950; body bytes: 1212 */

void __thiscall
FUN_004cc950(void *this,float *param_1,float param_2,float param_3,float param_4,undefined4 param_5,
            int param_6)

{
  char cVar1;
  float *pfVar2;
  int iVar3;
  undefined1 local_278 [12];
  undefined4 local_26c;
  undefined4 local_268;
  undefined4 local_264;
  code *local_260;
  undefined4 local_25c;
  int local_258;
  undefined4 local_254;
  int local_250;
  undefined4 local_24c;
  int local_248;
  int *local_244;
  int *local_240;
  float local_23c;
  int local_238;
  int local_234;
  int *local_230;
  int local_22c;
  int local_228;
  int local_224;
  float local_220;
  float local_21c;
  undefined4 local_218;
  float local_214;
  float local_210;
  undefined4 local_20c;
  undefined1 local_208 [512];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_230 = (int *)this;
  if ((param_6 == 0) || (*(int *)((int)this + 0x6cc) == 0)) {
    local_238 = 0;
    local_234 = 0;
    FUN_00422e10(&local_214);
    FUN_00422e10(&local_220);
    local_22c = local_230[0x16];
    local_228 = local_230[0x17];
    local_224 = local_230[0x18];
    local_23c = DAT_0056cd90 / DAT_0056c8d0;
    _memset(local_208,0,0x200);
    local_218 = 0;
    FUN_00439330(&local_220,&local_220,(float)local_230[0x1c],DAT_0056cd90 / DAT_0056c8d0);
    local_244 = local_230 + 0x16;
    pfVar2 = (float *)FUN_00429570(local_244,local_278,&local_220);
    local_214 = *pfVar2;
    local_210 = pfVar2[1];
    local_20c = 0;
    FUN_004296e0(&local_220,&local_220);
    for (; DAT_0056cd90 / DAT_0056c8d0 + local_23c < (float)local_230[0x1d];
        local_23c = local_23c + DAT_0056cd90) {
      cVar1 = FUN_004578b0(local_214,local_210,local_220,local_21c,(float)local_230[0x1c],*param_1,
                           param_1[1],param_2,param_3,param_4);
      if (cVar1 != '\0') {
        local_208[local_234] = 1;
        local_238 = local_238 + 1;
        iVar3 = FUN_0040c300((undefined4 *)local_230[0x1bc]);
        local_240 = (int *)(iVar3 + 0x286d88);
        *local_240 = *local_240 + 1;
        iVar3 = FUN_00485390(&local_214,DAT_0056cd94,DAT_0056cd94);
        if (iVar3 == 0) {
          if (((local_230[0x1b9] < 0x12) || (local_230[0x1b9] == 0x22)) ||
             (local_230[0x1b9] == 0x26)) {
            iVar3 = FUN_0040c300((undefined4 *)local_230[0x1bc]);
            local_24c = FUN_004859b0(iVar3);
            local_248 = local_230[0x1d4] * 2 + 0xd4;
            FUN_004790e0(&local_264,(uint *)"bullet",local_248,&local_214,0,-1,(undefined4 *)0x0);
          }
          else if ((local_230[0x1b9] < 0x20) || (local_230[0x1b9] == 0x1b)) {
            iVar3 = FUN_0040c300((undefined4 *)local_230[0x1bc]);
            local_254 = FUN_004859b0(iVar3);
            local_250 = local_230[0x1d4] * 2 + 0x104;
            FUN_004790e0(&local_268,(uint *)"bullet",local_250,&local_214,0,-1,(undefined4 *)0x0);
          }
          else if (local_230[0x1b9] < 0x22) {
            iVar3 = FUN_0040c300((undefined4 *)local_230[0x1bc]);
            local_25c = FUN_004859b0(iVar3);
            local_258 = local_230[0x1d4] * 2 + 0x11c;
            FUN_004790e0(&local_26c,(uint *)"bullet",local_258,&local_214,0,-1,(undefined4 *)0x0);
          }
        }
      }
      FUN_004296e0(&local_214,&local_220);
      local_234 = local_234 + 1;
    }
    if (local_238 != 0) {
      local_260 = *(code **)(*local_230 + 0x38);
      (*local_260)(local_208,local_234);
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

