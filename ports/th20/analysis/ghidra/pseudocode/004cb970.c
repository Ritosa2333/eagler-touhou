/* Entry: 0x004cb970; symbol: FUN_004cb970; body bytes: 1565 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall
FUN_004cb970(void *this,float *param_1,void *param_2,float param_3,undefined4 param_4,int param_5,
            int *param_6)

{
  char cVar1;
  float *pfVar2;
  int iVar3;
  void *this_00;
  float10 fVar4;
  float fVar5;
  undefined1 local_2bc [12];
  undefined1 local_2b0 [12];
  undefined1 local_2a4 [12];
  float local_298;
  float local_294;
  float local_290;
  float local_28c;
  void *local_288;
  void *local_284;
  void *local_280;
  void *local_27c;
  void *local_278;
  void *local_274;
  void *local_270;
  void *local_26c;
  int *local_268;
  int local_264;
  int local_260;
  float local_25c;
  int local_258;
  void *local_254;
  undefined4 local_250;
  undefined4 local_24c;
  undefined4 local_248;
  float local_244 [2];
  undefined4 local_23c;
  float local_238 [2];
  undefined4 local_230;
  float local_22c;
  float local_228;
  undefined4 local_224;
  float local_220;
  float local_21c;
  float local_218;
  float local_214;
  float local_210;
  undefined4 local_20c;
  undefined1 local_208 [512];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_254 = this;
  if ((param_5 == 0) || (*(int *)((int)this + 0x6cc) == 0)) {
    local_264 = 0;
    local_260 = 0;
    FUN_00422e10(&local_22c);
    FUN_00422e10(&local_220);
    FUN_00422e10(&local_214);
    FUN_00422e10(local_238);
    FUN_00422e10(local_244);
    local_250 = *(undefined4 *)((int)local_254 + 0x58);
    local_24c = *(undefined4 *)((int)local_254 + 0x5c);
    local_248 = *(undefined4 *)((int)local_254 + 0x60);
    local_25c = DAT_0056cd90 / DAT_0056c8d0;
    _memset(local_208,0,0x200);
    local_26c = (void *)((int)local_254 + 0x58);
    pfVar2 = (float *)FUN_00429440(local_26c,local_2a4,param_1);
    local_214 = *pfVar2;
    local_210 = pfVar2[1];
    local_20c = 0;
    FUN_00458fa0(&local_214,&local_214,&local_214,param_3);
    fVar5 = DAT_0056cd90 / DAT_0056c8d0;
    fVar4 = FUN_00438540(*(float *)((int)local_254 + 0x70) + param_3);
    FUN_00439330(local_244,local_244,(float)fVar4,fVar5);
    local_23c = 0;
    pfVar2 = (float *)FUN_00452ff0(param_2,local_2b0,DAT_0056c8d0);
    local_220 = *pfVar2;
    local_21c = pfVar2[1];
    local_218 = pfVar2[2];
    FUN_004296e0(&local_214,local_244);
    FUN_00439330(this_00,local_238,*(float *)((int)local_254 + 0x70),DAT_0056cd90 / DAT_0056c8d0);
    local_230 = 0;
    local_270 = (void *)((int)local_254 + 0x58);
    pfVar2 = (float *)FUN_00429570(local_270,local_2bc,local_238);
    local_22c = *pfVar2;
    local_228 = pfVar2[1];
    local_224 = 0;
    FUN_004296e0(local_238,local_238);
    for (; DAT_0056cd90 / DAT_0056c8d0 + local_25c <= *(float *)((int)local_254 + 0x74);
        local_25c = local_25c + DAT_0056cd90) {
      if (*param_6 == 0) {
        local_274 = (void *)FUN_00412730(*(int *)((int)local_254 + 0x6f0));
        local_278 = (void *)FUN_00485660(local_274,0);
        iVar3 = FUN_004d6b80(local_278,0);
        iVar3 = FUN_00437cd0(iVar3);
        local_28c = *(float *)(iVar3 + 0x48) * _DAT_00571788;
        local_27c = (void *)FUN_00412730(*(int *)((int)local_254 + 0x6f0));
        local_280 = (void *)FUN_00485660(local_27c,0);
        iVar3 = FUN_004d6b80(local_280,0);
        iVar3 = FUN_00437cd0(iVar3);
        local_290 = *(float *)(iVar3 + 0x4c) * _DAT_00571788;
        local_284 = (void *)FUN_00412730(*(int *)((int)local_254 + 0x6f0));
        iVar3 = FUN_00485660(local_284,0);
        iVar3 = FUN_0047a2c0(iVar3);
        local_294 = *(float *)(iVar3 + 4);
        local_288 = (void *)FUN_00412730(*(int *)((int)local_254 + 0x6f0));
        iVar3 = FUN_00485660(local_288,0);
        pfVar2 = (float *)FUN_0047a2c0(iVar3);
        local_298 = *pfVar2;
        cVar1 = FUN_00457610(local_298,local_294,local_290,local_28c,0.0,local_22c,local_228,
                             DAT_0056cd90 / DAT_0056c8d0);
        if (cVar1 != '\0') {
          *param_6 = 1;
        }
      }
      if (((((float)((uint)local_220 ^ _DAT_0056d7d0) <= local_214) && (local_214 <= local_220)) &&
          ((float)((uint)local_21c ^ _DAT_0056d7d0) <= local_210)) && (local_210 <= local_21c)) {
        local_208[local_260] = 1;
        local_264 = local_264 + 1;
        local_258 = 0x12;
        if (*(float *)((int)local_254 + 0x78) < DAT_0056fe7c) {
          if ((DAT_0056cd90 <= *(float *)((int)local_254 + 0x78)) &&
             (*(float *)((int)local_254 + 0x78) <= DAT_0056fe7c &&
              DAT_0056fe7c != *(float *)((int)local_254 + 0x78))) {
            local_258 = (int)(((*(float *)((int)local_254 + 0x78) - DAT_0056cd90) / DAT_0056fa30) *
                              DAT_0056c8d8 + 18.0 + DAT_0056c8cc);
          }
        }
        else {
          local_258 = (int)(((DAT_0056fe7c - DAT_0056cd90) / DAT_0056fa30) * DAT_0056c8d8 + 18.0 +
                           DAT_0056c8cc);
        }
        iVar3 = FUN_0041ca90(*(int *)((int)local_254 + 0x6f0));
        local_268 = (int *)(iVar3 + 0x4c);
        *local_268 = *local_268 + local_258;
      }
      FUN_004296e0(&local_22c,local_238);
      FUN_004296e0(&local_214,local_244);
      local_260 = local_260 + 1;
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

