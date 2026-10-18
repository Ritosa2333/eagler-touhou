/* Entry: 0x004cb060; symbol: FUN_004cb060; body bytes: 1165 */

void __thiscall FUN_004cb060(void *this,float *param_1,float param_2,undefined4 param_3,int param_4)

{
  float *pfVar1;
  int iVar2;
  uint uVar3;
  float10 fVar4;
  undefined1 local_280 [12];
  undefined4 local_274;
  undefined4 local_270;
  undefined4 local_26c;
  code *local_268;
  undefined4 local_264;
  int local_260;
  undefined4 local_25c;
  int local_258;
  undefined4 local_254;
  int local_250;
  float local_24c;
  int *local_248;
  int *local_244;
  float local_240;
  uint local_23c;
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
    local_234 = 0;
    local_238 = 0;
    FUN_00422e10(&local_214);
    FUN_00422e10(local_220);
    local_22c = local_230[0x16];
    local_228 = local_230[0x17];
    local_224 = local_230[0x18];
    local_240 = DAT_0056cd90 / DAT_0056c8d0;
    _memset(local_208,0,0x200);
    local_218 = 0;
    FUN_00439330(local_220,local_220,(float)local_230[0x1c],DAT_0056cd90 / DAT_0056c8d0);
    local_248 = local_230 + 0x16;
    pfVar1 = (float *)FUN_00429570(local_248,local_280,local_220);
    local_214 = *pfVar1;
    local_210 = pfVar1[1];
    local_20c = 0;
    FUN_004296e0(local_220,local_220);
    local_23c = 0;
    for (; DAT_0056cd90 / DAT_0056c8d0 + local_240 <= (float)local_230[0x1d];
        local_240 = local_240 + DAT_0056cd90) {
      fVar4 = FUN_004560d0(param_1,&local_214);
      local_24c = (float)fVar4;
      if (local_24c <= param_2 * param_2) {
        local_208[local_238] = 1;
        local_234 = local_234 + 1;
        iVar2 = FUN_0040c300((undefined4 *)local_230[0x1bc]);
        local_244 = (int *)(iVar2 + 0x286d88);
        *local_244 = *local_244 + 1;
        uVar3 = local_23c & 0x80000003;
        if ((int)uVar3 < 0) {
          uVar3 = (uVar3 - 1 | 0xfffffffc) + 1;
        }
        if (uVar3 == 0) {
          if (((local_230[0x1b9] < 0x12) || (local_230[0x1b9] == 0x22)) ||
             (local_230[0x1b9] == 0x26)) {
            iVar2 = FUN_0040c300((undefined4 *)local_230[0x1bc]);
            local_254 = FUN_004859b0(iVar2);
            local_250 = local_230[0x1c8] * 2 + 0xd4;
            FUN_004790e0(&local_26c,(uint *)"bullet",local_250,&local_214,0,-1,(undefined4 *)0x0);
          }
          else if ((local_230[0x1b9] < 0x20) || (local_230[0x1b9] == 0x1b)) {
            iVar2 = FUN_0040c300((undefined4 *)local_230[0x1bc]);
            local_25c = FUN_004859b0(iVar2);
            local_258 = local_230[0x1c8] * 2 + 0x104;
            FUN_004790e0(&local_270,(uint *)"bullet",local_258,&local_214,0,-1,(undefined4 *)0x0);
          }
          else if (local_230[0x1b9] < 0x22) {
            iVar2 = FUN_0040c300((undefined4 *)local_230[0x1bc]);
            local_264 = FUN_004859b0(iVar2);
            local_260 = local_230[0x1c8] * 2 + 0x11c;
            FUN_004790e0(&local_274,(uint *)"bullet",local_260,&local_214,0,-1,(undefined4 *)0x0);
          }
        }
        local_23c = local_23c + 1;
      }
      FUN_004296e0(&local_214,local_220);
      local_238 = local_238 + 1;
    }
    if (local_234 != 0) {
      if (local_234 < local_238) {
        local_268 = *(code **)(*local_230 + 0x38);
        (*local_268)(local_208,local_238);
      }
      else {
        local_230[7] = local_230[7] & 0xfffffff9U | 2;
      }
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

