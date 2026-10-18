/* Entry: 0x004e8390; symbol: FUN_004e8390; body bytes: 443 */

void __cdecl FUN_004e8390(uint *param_1,uint *param_2,float param_3)

{
  int *unaff_FS_OFFSET;
  bool bVar1;
  float10 fVar2;
  uint *local_24;
  uint local_20;
  float local_18;
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00569710;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  std::_Adl_verify_range<char*,char_const*>((char **)&param_1,(char **)&param_2);
  local_18 = (float)FUN_0040c990(&param_3);
  bVar1 = ((uint)local_18 & 0x80000000) != 0;
  if (bVar1) {
    if (param_1 == param_2) goto LAB_004e8532;
    *(undefined1 *)param_1 = 0x2d;
    param_1 = (uint *)((int)param_1 + 1);
    local_18 = ABS(local_18);
    fVar2 = FUN_00468010(&local_18);
    param_3 = (float)fVar2;
  }
  if (((uint)local_18 & 0x7f800000) == 0x7f800000) {
    if (((uint)local_18 & 0x7fffff) == 0) {
      local_24 = (uint *)&DAT_0056f57c;
      local_20 = 3;
    }
    else if ((bVar1) && (((uint)local_18 & 0x7fffff) == 0x400000)) {
      local_24 = (uint *)0x56f580;
      local_20 = 8;
    }
    else if (((uint)local_18 & 0x400000) == 0) {
      local_24 = (uint *)0x56f590;
      local_20 = 9;
    }
    else {
      local_24 = (uint *)&DAT_0056f58c;
      local_20 = 3;
    }
    if ((int)local_20 <= (int)param_2 - (int)param_1) {
      FUN_00543e20(param_1,local_24,local_20);
    }
  }
  else {
    FUN_004e9500(param_1,param_2,param_3,0);
  }
LAB_004e8532:
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

