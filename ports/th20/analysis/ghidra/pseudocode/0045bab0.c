/* Entry: 0x0045bab0; symbol: FUN_0045bab0; body bytes: 71 */

undefined4 __thiscall FUN_0045bab0(void *this,undefined4 param_1)

{
  float10 fVar1;
  
  fVar1 = FUN_0045bd10((int)this);
  *(undefined4 *)(*(int *)((int)this + 0x10) + 0x90) = param_1;
  FUN_0045beb0(this,(double)fVar1);
  return 0;
}

