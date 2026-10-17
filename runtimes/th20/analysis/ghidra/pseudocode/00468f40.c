/* Entry: 0x00468f40; symbol: FUN_00468f40; body bytes: 144 */

undefined8 __cdecl FUN_00468f40(uint *param_1,uint *param_2,float param_3,size_t param_4)

{
  int *unaff_FS_OFFSET;
  undefined8 uVar1;
  uint *local_20;
  undefined4 local_1c;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_005674f0;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  if ((int)param_4 < 0) {
    param_4 = 6;
  }
  else if (999999999 < (int)param_4) {
    local_1c = 0x84;
    local_20 = param_2;
    goto LAB_00468fc1;
  }
  uVar1 = FUN_0046f100(param_1,param_2,SUB84((double)param_3,0),
                       (int)((ulonglong)(double)param_3 >> 0x20),param_4);
  local_20 = (uint *)uVar1;
  local_1c = (undefined4)((ulonglong)uVar1 >> 0x20);
LAB_00468fc1:
  *unaff_FS_OFFSET = local_10;
  return CONCAT44(local_1c,local_20);
}

