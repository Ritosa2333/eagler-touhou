/* Entry: 0x004e6c00; symbol: FUN_004e6c00; body bytes: 45 */

undefined4 * __thiscall FUN_004e6c00(void *this,int param_1,int param_2)

{
  undefined4 uVar1;
  
  uVar1 = move<>(param_1);
  *(undefined4 *)this = uVar1;
  *(int *)((int)this + 4) = param_2 - param_1;
  return (undefined4 *)this;
}

