/* Entry: 0x004e80a0; symbol: FUN_004e80a0; body bytes: 721 */

void __cdecl
FUN_004e80a0(uint *param_1,uint *param_2,double param_3,undefined4 param_4,size_t param_5)

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
  puStack_c = &LAB_0056a100;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  std::_Adl_verify_range<char*,char_const*>((char **)&param_1,(char **)&param_2);
  local_1c = (double)FUN_004e7ae0(&param_3);
  bVar1 = ((ulonglong)local_1c & 0x8000000000000000) != 0;
  if (bVar1) {
    if (param_1 == param_2) goto LAB_004e8358;
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
  else {
    switch(param_4) {
    case 1:
      FUN_004e9600(param_1,param_2,param_3,param_5);
      break;
    case 2:
      FUN_004e8770((char *)param_1,(char *)param_2,param_3,param_5);
      break;
    case 3:
      FUN_004e8820(param_1,param_2,param_3,param_5);
      break;
    default:
      FUN_004e8d20((char *)param_1,(char *)param_2,SUB84(param_3,0),
                   (int)((ulonglong)param_3 >> 0x20),param_5);
    }
  }
LAB_004e8358:
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

