/* Entry: 0x0044e8f0; symbol: FUN_0044e8f0; body bytes: 562 */

void FUN_0044e8f0(undefined4 *param_1,int param_2,undefined4 param_3,int param_4,int param_5,
                 int param_6,int param_7,int param_8,int param_9)

{
  undefined4 uVar1;
  int iVar2;
  undefined4 uVar3;
  int iVar4;
  undefined4 local_38;
  int local_34;
  int local_30;
  int *local_2c;
  undefined4 local_28;
  undefined4 local_24;
  int local_20;
  int local_1c;
  int local_18;
  int local_14;
  int local_10;
  int local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  iVar2 = FUN_0044cfb0(param_2);
  param_1[5] = param_1[5] & 0xfffffffc;
  if (param_8 < param_4) {
    local_30 = param_8;
  }
  else {
    local_30 = param_4;
  }
  if (param_9 < param_5) {
    local_34 = param_9;
  }
  else {
    local_34 = param_5;
  }
  local_18 = param_6;
  local_14 = param_7;
  local_10 = param_6 + local_30;
  local_c = param_7 + local_34;
  if ((*(char *)(param_1[4] + 0x22) != '\0') && (DAT_005b8818 < DAT_0056c8d0)) {
    param_4 = (int)((float)param_4 * DAT_005b8818 * DAT_0056e0ec);
    param_5 = (int)((float)param_5 * DAT_005b8818 * DAT_0056e0ec);
  }
  local_28 = 0;
  local_24 = 0;
  local_20 = param_4;
  local_1c = param_5;
  uVar1 = *(undefined4 *)(&DAT_0056e208 + iVar2 * 4);
  uVar3 = FUN_00412730(0x5c4d40);
  D3DXCreateTexture(uVar3,param_4,param_5,1,0,uVar1,1,param_1);
  local_2c = (int *)0x0;
  (**(code **)(*(int *)*param_1 + 0x48))(*param_1,0,&local_2c);
  if ((*(char *)(param_1[4] + 0x22) == '\0') || (DAT_0056c8d0 <= DAT_005b8818)) {
    local_38 = 1;
  }
  else {
    local_38 = 0x30004;
  }
  iVar4 = D3DXLoadSurfaceFromFileInMemory
                    (local_2c,0,&local_28,param_1[1],param_1[2],&local_18,local_38,0,0);
  if (iVar4 == 0) {
    if (local_2c != (int *)0x0) {
      (**(code **)(*local_2c + 8))(local_2c);
      local_2c = (int *)0x0;
    }
    FUN_00451910((int *)*param_1);
    param_1[3] = *(undefined4 *)(&DAT_0056e22c + iVar2 * 4);
  }
  else if (local_2c != (int *)0x0) {
    (**(code **)(*local_2c + 8))(local_2c);
    local_2c = (int *)0x0;
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

