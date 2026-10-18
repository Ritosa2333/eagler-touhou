/* Entry: 0x004613d0; symbol: FUN_004613d0; body bytes: 208 */

undefined4 __thiscall FUN_004613d0(void *this,int param_1)

{
  float10 fVar1;
  undefined4 local_30 [2];
  undefined4 local_28 [2];
  void *local_20;
  undefined4 *local_1c;
  void *local_18;
  float local_14;
  float local_10;
  int local_c;
  void *local_8;
  
  local_c = param_1;
  if (param_1 == 1) {
    local_18 = *(void **)((int)this + 4);
    local_8 = this;
    FUN_00429250(local_28,0,0);
    fVar1 = FUN_004379c0(*(int *)((int)local_8 + 4));
    local_14 = (float)fVar1;
    fVar1 = FUN_004379a0(*(int *)((int)local_8 + 4));
    local_10 = (float)fVar1;
    FUN_00429250(local_30,local_14,local_10);
    FUN_00438df0(local_18,0x14,4,local_30,local_28);
    local_20 = *(void **)((int)local_8 + 4);
    local_1c = (undefined4 *)FUN_0044cb60(*(int *)((int)local_8 + 4));
    FUN_004614a0(local_20,0x14,4,local_1c,(undefined4 *)((int)local_8 + 8));
    FUN_00423520((void *)((int)local_8 + 0x38),0x14);
  }
  return 0;
}

