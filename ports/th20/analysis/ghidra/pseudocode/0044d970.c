/* Entry: 0x0044d970; symbol: FUN_0044d970; body bytes: 90 */

undefined4 __thiscall FUN_0044d970(void *this,uint param_1)

{
  uint uVar1;
  
  uVar1 = FUN_004494d0((void *)((int)this + 0x710),0);
  if ((uVar1 <= param_1) &&
     (uVar1 = FUN_004494d0((void *)((int)this + 0x710),0xffff), param_1 <= uVar1)) {
    return 1;
  }
  return 0;
}

