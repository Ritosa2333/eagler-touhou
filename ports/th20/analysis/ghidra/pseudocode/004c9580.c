/* Entry: 0x004c9580; symbol: FUN_004c9580; body bytes: 683 */

void __thiscall FUN_004c9580(void *this,undefined4 param_1,int param_2)

{
  float *pfVar1;
  int iVar2;
  undefined1 local_60 [12];
  undefined4 local_54;
  undefined4 local_50;
  undefined4 local_4c;
  undefined4 local_48;
  int local_44;
  undefined4 local_40;
  int local_3c;
  undefined4 local_38;
  int local_34;
  void *local_30;
  int local_2c;
  float local_28;
  void *local_24;
  float local_20 [2];
  undefined4 local_18;
  float local_14;
  float local_10;
  float local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_24 = this;
  if ((param_2 == 0) || (*(int *)((int)this + 0x6cc) == 0)) {
    local_2c = 0;
    FUN_00422e10(&local_14);
    FUN_00422e10(local_20);
    local_28 = DAT_0056cd90 / DAT_0056c8d0;
    FUN_00439330(local_24,local_20,*(float *)((int)local_24 + 0x70),DAT_0056cd90 / DAT_0056c8d0);
    local_18 = 0;
    local_30 = (void *)((int)local_24 + 0x58);
    pfVar1 = (float *)FUN_00429570(local_30,local_60,local_20);
    local_14 = *pfVar1;
    local_10 = pfVar1[1];
    local_c = pfVar1[2];
    FUN_004296e0(local_20,local_20);
    for (; DAT_0056cd90 / DAT_0056c8d0 + local_28 < *(float *)((int)local_24 + 0x74);
        local_28 = local_28 + DAT_0056cd90) {
      local_2c = local_2c + 1;
      iVar2 = FUN_00485390(&local_14,DAT_0056cd90,DAT_0056cd90);
      if (iVar2 == 0) {
        if (((*(int *)((int)local_24 + 0x6e4) < 0x12) || (*(int *)((int)local_24 + 0x6e4) == 0x22))
           || (*(int *)((int)local_24 + 0x6e4) == 0x26)) {
          iVar2 = FUN_0040c300(*(undefined4 **)((int)local_24 + 0x6f0));
          local_38 = FUN_004859b0(iVar2);
          local_34 = *(int *)((int)local_24 + 0x750) * 2 + 0xd4;
          FUN_004790e0(&local_4c,(uint *)"bullet",local_34,&local_14,0,-1,(undefined4 *)0x0);
        }
        else if ((*(int *)((int)local_24 + 0x6e4) < 0x20) ||
                (*(int *)((int)local_24 + 0x6e4) == 0x1b)) {
          iVar2 = FUN_0040c300(*(undefined4 **)((int)local_24 + 0x6f0));
          local_40 = FUN_004859b0(iVar2);
          local_3c = *(int *)((int)local_24 + 0x750) * 2 + 0x104;
          FUN_004790e0(&local_50,(uint *)"bullet",local_3c,&local_14,0,-1,(undefined4 *)0x0);
        }
        else if (*(int *)((int)local_24 + 0x6e4) < 0x22) {
          iVar2 = FUN_0040c300(*(undefined4 **)((int)local_24 + 0x6f0));
          local_48 = FUN_004859b0(iVar2);
          local_44 = *(int *)((int)local_24 + 0x750) * 2 + 0x11c;
          FUN_004790e0(&local_54,(uint *)"bullet",local_44,&local_14,0,-1,(undefined4 *)0x0);
        }
      }
      FUN_004296e0(&local_14,local_20);
    }
    *(undefined4 *)((int)local_24 + 0x20) = 1;
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

