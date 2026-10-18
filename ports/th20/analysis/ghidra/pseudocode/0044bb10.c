/* Entry: 0x0044bb10; symbol: FUN_0044bb10; body bytes: 439 */

void __thiscall FUN_0044bb10(void *this,int *param_1)

{
  int *piVar1;
  int iVar2;
  int *local_2c;
  int local_28;
  int local_24;
  int local_20;
  int local_1c;
  int local_18;
  int local_14;
  int local_10;
  int local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  piVar1 = (int *)FUN_00414580((void *)((int)this + 0x6000730),*param_1);
  if (*(int *)(*(int *)(*piVar1 + 0x58) + param_1[1] * 0x18) != 0) {
    FUN_004455c0((int)this);
    piVar1 = (int *)FUN_00414580((void *)((int)this + 0x6000730),*param_1);
    iVar2 = (**(code **)(**(int **)(*(int *)(*piVar1 + 0x58) + param_1[1] * 0x18) + 0x48))
                      (*(undefined4 *)(*(int *)(*piVar1 + 0x58) + param_1[1] * 0x18),0,&local_2c);
    if (iVar2 == 0) {
      local_18 = param_1[2];
      local_14 = param_1[3];
      local_10 = param_1[2] + param_1[4];
      local_c = param_1[3] + param_1[5];
      local_28 = param_1[6];
      local_24 = param_1[7];
      local_20 = param_1[6] + param_1[8];
      local_1c = param_1[7] + param_1[9];
      iVar2 = D3DXLoadSurfaceFromSurface(local_2c,0,&local_28,DAT_005c4ee4,0,&local_18,2,0);
      if (iVar2 == 0) {
        piVar1 = (int *)FUN_00414580((void *)((int)this + 0x6000730),*param_1);
        (**(code **)(**(int **)(*(int *)(*piVar1 + 0x58) + param_1[1] * 0x18) + 0x54))
                  (*(undefined4 *)(*(int *)(*piVar1 + 0x58) + param_1[1] * 0x18),0);
        (**(code **)(*local_2c + 8))(local_2c);
      }
      else {
        (**(code **)(*local_2c + 8))(local_2c);
      }
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

