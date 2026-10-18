/* Entry: 0x004659a0; symbol: FUN_004659a0; body bytes: 743 */

void __thiscall FUN_004659a0(void *this,undefined4 param_1)

{
  void *pvVar1;
  int local_54;
  int local_50;
  int local_4c;
  int local_48;
  int local_40;
  undefined4 local_38 [3];
  undefined4 local_2c [3];
  undefined4 local_20 [3];
  undefined4 local_14 [3];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  switch(param_1) {
  case 1:
    for (local_40 = 0; local_40 < 4; local_40 = local_40 + 1) {
      pvVar1 = (void *)FUN_00437520(0);
      pvVar1 = (void *)FUN_0045d100(pvVar1,2);
      FUN_004382b0(pvVar1,(void *)((int)this + local_40 * 0x5e4 + 8),local_40 + 7,0);
    }
    *(undefined4 *)((int)this + 0x1d84) = 1;
    *(undefined4 *)((int)this + 0x1d80) = 0;
    break;
  case 7:
    *(undefined4 *)((int)this + 0x1d7c) = 0;
    FUN_00450350((void *)((int)this + 8),0x23);
    FUN_00422dd0(local_14,DAT_0056f10c,DAT_0056f108,0);
    for (local_48 = 0; local_48 < 4; local_48 = local_48 + 1) {
      FUN_0045dd60((void *)((int)this + local_48 * 0x5e4 + 8),local_14);
    }
    break;
  case 8:
    *(undefined4 *)((int)this + 0x1d7c) = 1;
    FUN_00450350((void *)((int)this + 8),0x17);
    FUN_00422dd0(local_20,DAT_0056f10c,DAT_0056f108,0);
    for (local_4c = 0; local_4c < 4; local_4c = local_4c + 1) {
      FUN_0045dd60((void *)((int)this + local_4c * 0x5e4 + 8),local_20);
    }
    break;
  case 9:
    *(undefined4 *)((int)this + 0x1d7c) = 0;
    FUN_00450350((void *)((int)this + 8),0x2d);
    FUN_00422dd0(local_2c,DAT_0056f10c,DAT_0056f108,0);
    for (local_50 = 0; local_50 < 4; local_50 = local_50 + 1) {
      FUN_0045dd60((void *)((int)this + local_50 * 0x5e4 + 8),local_2c);
    }
    break;
  case 10:
    *(undefined4 *)((int)this + 0x1d7c) = 3;
    FUN_00450350((void *)((int)this + 8),0x23);
    FUN_00422dd0(local_38,DAT_0056f10c,DAT_0056f108,0);
    for (local_54 = 0; local_54 < 4; local_54 = local_54 + 1) {
      FUN_0045dd60((void *)((int)this + local_54 * 0x5e4 + 8),local_38);
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

