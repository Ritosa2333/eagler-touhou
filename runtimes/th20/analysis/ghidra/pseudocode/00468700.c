/* Entry: 0x00468700; symbol: FUN_00468700; body bytes: 157 */

undefined8 __cdecl FUN_00468700(char *param_1,char *param_2,float param_3,size_t param_4)

{
  undefined8 *puVar1;
  int *unaff_FS_OFFSET;
  undefined8 uVar2;
  undefined1 local_28 [8];
  char *local_20;
  undefined4 local_1c;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_005682e0;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  if ((int)param_4 < 0) {
    param_4 = 6;
  }
  else if (999999999 < (int)param_4) {
    local_1c = 0x84;
    local_20 = param_2;
    goto LAB_0046878e;
  }
  puVar1 = (undefined8 *)FUN_00469720(local_28,param_1,param_2,(double)param_3,param_4);
  uVar2 = FUN_0046eaf0(puVar1);
  local_20 = (char *)uVar2;
  local_1c = (undefined4)((ulonglong)uVar2 >> 0x20);
LAB_0046878e:
  *unaff_FS_OFFSET = local_10;
  return CONCAT44(local_1c,local_20);
}

