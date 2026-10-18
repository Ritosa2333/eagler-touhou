/* Entry: 0x0043cb70; symbol: FUN_0043cb70; body bytes: 455 */

undefined4 __thiscall
FUN_0043cb70(void *this,int param_1,float *param_2,float *param_3,float *param_4)

{
  int *piVar1;
  int local_10;
  float *local_c;
  
  if (((uint)((param_1 + 1) * 0x14 + *(int *)((int)this + 0x7d40e80)) < (int)this + 0x7d40e80U) &&
     (2 < param_1)) {
    local_c = *(float **)((int)this + 0x7d40e80);
    FUN_004455c0((int)this);
    for (local_10 = 0; local_10 < param_1; local_10 = local_10 + 1) {
      *local_c = *param_3 + *param_2;
      local_c[1] = param_3[1] + param_2[1];
      local_c[2] = 0.0;
      local_c[3] = DAT_0056c8cc;
      local_c[4] = *param_4;
      *local_c = *local_c + *(float *)((int)this + 0xd0);
      local_c[1] = local_c[1] + *(float *)((int)this + 0xd4);
      param_3 = param_3 + 2;
      param_4 = param_4 + 1;
      local_c = local_c + 5;
    }
    *(undefined1 *)((int)this + 0x6000e0e) = 1;
    FUN_00446690(this,'\x02');
    piVar1 = (int *)FUN_00412730(0x5c4d40);
    (**(code **)(*piVar1 + 0x164))(piVar1,0x44);
    piVar1 = (int *)FUN_00412730(0x5c4d40);
    (**(code **)(*piVar1 + 0x14c))
              (piVar1,6,param_1 + -2,*(undefined4 *)((int)this + 0x7d40e80),0x14);
    *(int *)((int)this + 0x7d40e80) = param_1 * 0x14 + *(int *)((int)this + 0x7d40e80);
    *(int *)((int)this + 0xc4) = *(int *)((int)this + 0xc4) + 1;
  }
  return 0;
}

