/* Entry: 0x004c1b80; symbol: FUN_004c1b80; body bytes: 140 */

undefined4 * __thiscall
FUN_004c1b80(void *this,undefined4 *param_1,undefined4 *param_2,undefined4 param_3,
            undefined4 param_4,int param_5,undefined4 param_6)

{
  void *this_00;
  
  FUN_004142a0(param_1,*(undefined4 *)((int)this + 0xc410));
  this_00 = FUN_004c0e60((int)this);
  if (this_00 == (void *)0x0) {
    FUN_004117a0(param_1,0);
  }
  else {
    FUN_00488a20(this_00,*(undefined4 *)((int)this + 0xc458));
    FUN_004c1d80(this_00,param_2,param_3,param_4,param_5,param_6);
    *param_1 = *(undefined4 *)((int)this_00 + 0x8c);
  }
  return param_1;
}

