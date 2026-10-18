/* Entry: 0x004e7e40; symbol: FUN_004e7e40; body bytes: 588 */

void __cdecl FUN_004e7e40(uint *param_1,uint *param_2,double param_3,int param_4)

{
  bool bVar1;
  int *unaff_FS_OFFSET;
  float10 fVar2;
  uint *local_28;
  uint local_24;
  undefined8 local_1c;
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00568760;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  std::_Adl_verify_range<char*,char_const*>((char **)&param_1,(char **)&param_2);
  local_1c = (double)FUN_004e7ae0(&param_3);
  bVar1 = ((ulonglong)local_1c & 0x8000000000000000) != 0;
  if (bVar1) {
    if (param_1 == param_2) goto LAB_004e8073;
    *(undefined1 *)param_1 = 0x2d;
    param_1 = (uint *)((int)param_1 + 1);
    local_1c = ABS(local_1c);
    fVar2 = FUN_004e7ac0((double *)&local_1c);
    param_3 = (double)fVar2;
  }
  if ((local_1c._4_4_ & 0x7ff00000) == 0x7ff00000) {
    if ((int)local_1c == 0 && ((ulonglong)local_1c & 0xfffff00000000) == 0) {
      local_28 = (uint *)&DAT_0056f57c;
      local_24 = 3;
    }
    else if (((bVar1) && ((int)local_1c == 0)) && ((local_1c._4_4_ & 0xfffff) == 0x80000)) {
      local_28 = (uint *)0x56f580;
      local_24 = 8;
    }
    else if (((ulonglong)local_1c & 0x8000000000000) == 0) {
      local_28 = (uint *)0x56f590;
      local_24 = 9;
    }
    else {
      local_28 = (uint *)&DAT_0056f58c;
      local_24 = 3;
    }
    if ((int)local_24 <= (int)param_2 - (int)param_1) {
      FUN_00543e20(param_1,local_28,local_24);
    }
  }
  else if (param_4 == 4) {
    FUN_004e92a0(param_1,param_2);
  }
  else {
    FUN_004e9580(param_1,param_2,param_3,param_4);
  }
LAB_004e8073:
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

