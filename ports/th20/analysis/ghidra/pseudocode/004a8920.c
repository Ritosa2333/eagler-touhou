/* Entry: 0x004a8920; symbol: FUN_004a8920; body bytes: 155 */

undefined4 * __thiscall FUN_004a8920(void *this,char *param_1,undefined4 *param_2,int param_3)

{
  undefined4 *this_00;
  
  this_00 = FUN_004a2910();
  FUN_004a73f0(this_00,*(undefined4 *)((int)this + 300),param_1);
  if (param_3 != 0) {
    FUN_00411ea0((void *)(param_3 + 0x3cc),this_00 + 0xf9);
  }
  FUN_004a89c0(this_00,param_2);
  FUN_0044a6b0((void *)((int)this + 0x108),this_00 + 0x1d);
  *(int *)((int)this + 0x124) = *(int *)((int)this + 0x124) + 1;
  return this_00;
}

