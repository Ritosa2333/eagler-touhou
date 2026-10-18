/* Entry: 0x004cabf0; symbol: FUN_004cabf0; body bytes: 1118 */

void __thiscall FUN_004cabf0(void *this,float *param_1,float param_2,undefined4 param_3,int param_4)

{
  float *pfVar1;
  int iVar2;
  float10 fVar3;
  undefined1 local_27c [12];
  undefined4 local_270;
  undefined4 local_26c;
  undefined4 local_268;
  code *local_264;
  undefined4 local_260;
  int local_25c;
  undefined4 local_258;
  int local_254;
  undefined4 local_250;
  int local_24c;
  float local_248;
  int *local_244;
  int *local_240;
  float local_23c;
  int local_238;
  int local_234;
  int *local_230;
  int local_22c;
  int local_228;
  int local_224;
  float local_220 [2];
  undefined4 local_218;
  float local_214;
  float local_210;
  undefined4 local_20c;
  undefined1 local_208 [512];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_230 = (int *)this;
  if ((param_4 == 0) || (*(int *)((int)this + 0x6cc) == 0)) {
    local_238 = 0;
    local_234 = 0;
    FUN_00422e10(&local_214);
    FUN_00422e10(local_220);
    local_22c = local_230[0x16];
    local_228 = local_230[0x17];
    local_224 = local_230[0x18];
    local_23c = DAT_0056cd90 / DAT_0056c8d0;
    _memset(local_208,0,0x200);
    local_218 = 0;
    FUN_00439330(local_220,local_220,(float)local_230[0x1c],DAT_0056cd90 / DAT_0056c8d0);
    local_244 = local_230 + 0x16;
    pfVar1 = (float *)FUN_00429570(local_244,local_27c,local_220);
    local_214 = *pfVar1;
    local_210 = pfVar1[1];
    local_20c = 0;
    FUN_004296e0(local_220,local_220);
    for (; DAT_0056cd90 / DAT_0056c8d0 + local_23c < (float)local_230[0x1d];
        local_23c = local_23c + DAT_0056cd90) {
      fVar3 = FUN_004560d0(param_1,&local_214);
      local_248 = (float)fVar3;
      if (local_248 <= param_2 * param_2) {
        local_208[local_234] = 1;
        local_238 = local_238 + 1;
        iVar2 = FUN_0040c300((undefined4 *)local_230[0x1bc]);
        local_240 = (int *)(iVar2 + 0x286d88);
        *local_240 = *local_240 + 1;
        iVar2 = FUN_00485390(&local_214,DAT_0056cd94,DAT_0056cd94);
        if (iVar2 == 0) {
          if (((local_230[0x1b9] < 0x12) || (local_230[0x1b9] == 0x22)) ||
             (local_230[0x1b9] == 0x26)) {
            iVar2 = FUN_0040c300((undefined4 *)local_230[0x1bc]);
            local_250 = FUN_004859b0(iVar2);
            local_24c = local_230[0x1d4] * 2 + 0xd4;
            FUN_004790e0(&local_268,(uint *)"bullet",local_24c,&local_214,0,-1,(undefined4 *)0x0);
          }
          else if ((local_230[0x1b9] < 0x20) || (local_230[0x1b9] == 0x1b)) {
            iVar2 = FUN_0040c300((undefined4 *)local_230[0x1bc]);
            local_258 = FUN_004859b0(iVar2);
            local_254 = local_230[0x1d4] * 2 + 0x104;
            FUN_004790e0(&local_26c,(uint *)"bullet",local_254,&local_214,0,-1,(undefined4 *)0x0);
          }
          else if (local_230[0x1b9] < 0x22) {
            iVar2 = FUN_0040c300((undefined4 *)local_230[0x1bc]);
            local_260 = FUN_004859b0(iVar2);
            local_25c = local_230[0x1d4] * 2 + 0x11c;
            FUN_004790e0(&local_270,(uint *)"bullet",local_25c,&local_214,0,-1,(undefined4 *)0x0);
          }
        }
      }
      FUN_004296e0(&local_214,local_220);
      local_234 = local_234 + 1;
    }
    if (local_238 != 0) {
      local_264 = *(code **)(*local_230 + 0x38);
      (*local_264)(local_208,local_234);
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

