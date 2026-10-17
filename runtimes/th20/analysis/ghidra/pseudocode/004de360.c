/* Entry: 0x004de360; symbol: FUN_004de360; body bytes: 198 */

undefined4 * FUN_004de360(undefined4 *param_1,int param_2,int param_3)

{
  void *this;
  int *piVar1;
  undefined4 local_10;
  undefined4 local_8;
  
  FUN_00450c70(param_1,(uint *)0x0,param_3,0x2a,(undefined4 *)0x0);
  this = (void *)FUN_0044ced0(param_1);
  FUN_0042b590(this,param_2 * 0x38);
  if (param_2 < 3) {
    *(undefined1 *)((int)this + 0x498) = 0;
  }
  else {
    *(undefined1 *)((int)this + 0x498) = 0xc;
    piVar1 = (int *)FUN_0045d0c0((int)this);
    *piVar1 = param_2;
    local_8 = FUN_0049dff0((int)this);
    for (local_10 = 0; SBORROW4(local_10,param_2 * 2) != local_10 + param_2 * -2 < 0;
        local_10 = local_10 + 1) {
      *(undefined4 *)(local_8 + 8) = 0;
      *(undefined4 *)(local_8 + 0xc) = DAT_0056c8cc;
      *(undefined4 *)(local_8 + 0x10) = 0xffffffff;
      local_8 = local_8 + 0x1c;
    }
  }
  return param_1;
}

