/* Entry: 0x00412400; symbol: FUN_00412400; body bytes: 162 */

void __thiscall FUN_00412400(void *this,void *param_1)

{
  undefined4 uVar1;
  int local_8;
  
  if ((param_1 != (void *)0x0) &&
     ((local_8 = FUN_00411e70((void *)((int)this + 4),(int)param_1), local_8 != 0 ||
      (local_8 = FUN_00411e70((void *)((int)this + 0x1c),(int)param_1), local_8 != 0)))) {
                    /* WARNING: Load size is inaccurate */
    if (*this == local_8) {
      uVar1 = FUN_0040ff90(local_8);
      *(undefined4 *)this = uVar1;
    }
    FUN_00411ce0(local_8);
    *(undefined4 *)((int)param_1 + 8) = 0;
    if ((*(uint *)((int)param_1 + 4) & 1) != 0) {
      FUN_004118a0(param_1);
    }
  }
  return;
}

