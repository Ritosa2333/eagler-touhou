/* Entry: 0x0044d9d0; symbol: FUN_0044d9d0; body bytes: 574 */

void FUN_0044d9d0(undefined4 *param_1,int param_2,int param_3,int param_4,int param_5)

{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  undefined4 uVar4;
  int iVar5;
  int iVar6;
  undefined4 uVar7;
  undefined4 uVar8;
  undefined4 local_40;
  int *local_38;
  undefined1 local_34 [28];
  undefined4 local_18;
  undefined4 local_14;
  int local_10;
  int local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_38 = (int *)0x0;
  iVar2 = FUN_0044cfb0(param_3);
  param_1[5] = param_1[5] & 0xfffffffc;
  uVar7 = 0;
  uVar8 = 0;
  iVar3 = (int)*(short *)(param_2 + 8);
  iVar6 = (int)*(short *)(param_2 + 10);
  local_18 = 0;
  local_14 = 0;
  local_10 = (int)*(short *)(param_2 + 8);
  local_c = (int)*(short *)(param_2 + 10);
  if ((*(char *)(param_1[4] + 0x22) != '\0') && (DAT_005b8818 < DAT_0056c8d0)) {
    local_10 = (int)((float)local_10 * DAT_005b8818 * DAT_0056e0ec);
    local_c = (int)((float)local_c * DAT_005b8818 * DAT_0056e0ec);
    param_4 = (int)((float)param_4 * DAT_005b8818 * DAT_0056e0ec);
    param_5 = (int)((float)param_5 * DAT_005b8818 * DAT_0056e0ec);
  }
  uVar1 = *(undefined4 *)(&DAT_0056e208 + iVar2 * 4);
  uVar4 = FUN_00412730(0x5c4d40);
  iVar5 = D3DXCreateTexture(uVar4,param_4,param_5,1,0,uVar1,1,param_1);
  if (iVar5 == 0) {
    (**(code **)(*(int *)*param_1 + 0x48))(*param_1,0,&local_38,uVar7,uVar8,iVar3,iVar6);
    if ((*(char *)(param_1[4] + 0x22) == '\0') || (DAT_0056c8d0 <= DAT_005b8818)) {
      local_40 = 1;
    }
    else {
      local_40 = 0x30004;
    }
    D3DXLoadSurfaceFromFileInMemory
              (local_38,0,&local_18,param_2 + 0x10,*(undefined4 *)(param_2 + 0xc),0,local_40,0,
               local_34);
    param_1[3] = *(undefined4 *)(&DAT_0056e22c + iVar2 * 4);
    if (local_38 != (int *)0x0) {
      (**(code **)(*local_38 + 8))(local_38);
      local_38 = (int *)0x0;
    }
  }
  else if (local_38 != (int *)0x0) {
    (**(code **)(*local_38 + 8))(local_38);
    local_38 = (int *)0x0;
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

