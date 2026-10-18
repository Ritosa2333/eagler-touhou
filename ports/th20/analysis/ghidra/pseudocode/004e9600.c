/* Entry: 0x004e9600; symbol: FUN_004e9600; body bytes: 144 */

undefined8 __cdecl FUN_004e9600(uint *param_1,uint *param_2,undefined8 param_3,size_t param_4)

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
    goto LAB_004e9681;
  }
  uVar1 = FUN_0046f100(param_1,param_2,(int)param_3,(int)((ulonglong)param_3 >> 0x20),param_4);
  local_20 = (uint *)uVar1;
  local_1c = (undefined4)((ulonglong)uVar1 >> 0x20);
LAB_004e9681:
  *unaff_FS_OFFSET = local_10;
  return CONCAT44(local_1c,local_20);
}

