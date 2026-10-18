/* Entry: 0x0041a280; symbol: FUN_0041a280; body bytes: 61 */

undefined4 __thiscall FUN_0041a280(void *this,uint param_1)

{
  uint uVar1;
  undefined4 local_c;
  
  uVar1 = FUN_00419c00(this,param_1);
  if ((uVar1 == 0) && ((*(uint *)((int)this + 8) & param_1) == 0)) {
    local_c = 0;
  }
  else {
    local_c = 1;
  }
  return local_c;
}

