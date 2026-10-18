/* Entry: 0x004cd2d0; symbol: FUN_004cd2d0; body bytes: 683 */

void __thiscall
FUN_004cd2d0(void *this,float *param_1,undefined4 param_2,float param_3,int param_4,
            undefined4 param_5,int param_6)

{
  char cVar1;
  int iVar2;
  undefined4 local_244;
  code *local_240;
  undefined4 local_23c;
  int local_238;
  int *local_234;
  float *local_230;
  int local_22c;
  int local_228;
  int *local_224;
  int local_220;
  int local_21c;
  int local_218;
  float local_214;
  float local_210;
  float local_20c;
  undefined1 local_208 [512];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_224 = (int *)this;
  if ((param_6 == 0) || (*(int *)((int)this + 0x6cc) == 0)) {
    local_22c = 0;
    local_228 = 0;
    FUN_00422e10(&local_214);
    local_220 = local_224[0x16];
    local_21c = local_224[0x17];
    local_218 = local_224[0x18];
    _memset(local_208,0,0x200);
    local_230 = (float *)local_224[0x4ca];
    for (local_228 = 0; local_228 < local_224[0x1c6]; local_228 = local_228 + 1) {
      local_214 = *local_230;
      local_210 = local_230[1];
      local_20c = local_230[2];
      cVar1 = FUN_004570f0(local_214,local_210,*param_1,param_1[1],param_2,param_3,param_4);
      if (cVar1 != '\0') {
        local_208[local_228] = 1;
        local_22c = local_22c + 1;
        iVar2 = FUN_0040c300((undefined4 *)local_224[0x1bc]);
        local_234 = (int *)(iVar2 + 0x286d88);
        *local_234 = *local_234 + 1;
        if (local_228 % 0x14 == 0) {
          iVar2 = FUN_0040c300((undefined4 *)local_224[0x1bc]);
          local_23c = FUN_004859b0(iVar2);
          local_238 = local_224[0x1c5] * 2 + 0xd4;
          FUN_004790e0(&local_244,(uint *)"bullet",local_238,&local_214,0,-1,(undefined4 *)0x0);
        }
      }
      local_230 = local_230 + 8;
    }
    if (local_22c < local_224[0x1c6]) {
      if (0 < local_22c) {
        local_240 = *(code **)(*local_224 + 0x38);
        (*local_240)(local_208,local_228);
      }
    }
    else {
      local_224[7] = local_224[7] & 0xfffffff9U | 2;
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

