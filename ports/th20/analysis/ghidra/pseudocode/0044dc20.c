/* Entry: 0x0044dc20; symbol: FUN_0044dc20; body bytes: 293 */

void FUN_0044dc20(undefined4 *param_1,int param_2,undefined4 param_3,int param_4,undefined4 param_5,
                 int param_6)

{
  int iVar1;
  int *local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  iVar1 = FUN_0044cfb0(param_4);
  param_1[2] = param_3;
  param_1[5] = param_1[5] & 0xfffffffc;
  local_c = (int *)0x0;
  (**(code **)(*(int *)*param_1 + 0x48))(*param_1,0,&local_c);
  if (param_6 == 0) {
    D3DXLoadSurfaceFromFileInMemory(local_c,0,0,param_2,param_3,0,1,0,0);
  }
  else {
    D3DXLoadSurfaceFromFileInMemory
              (local_c,0,0,param_2 + 0x10 + *(int *)(param_2 + 0x1c),param_3,0,1,0,0);
  }
  (**(code **)(*local_c + 8))(local_c);
  FUN_00451910((int *)*param_1);
  param_1[3] = *(undefined4 *)(&DAT_0056e22c + iVar1 * 4);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

