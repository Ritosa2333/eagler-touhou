/* Entry: 0x0053f7b0; symbol: FUN_0053f7b0; body bytes: 89 */

undefined4 * __thiscall FUN_0053f7b0(void *this,undefined4 param_1,undefined4 param_2)

{
  undefined4 uVar1;
  int iVar2;
  undefined4 uVar3;
  
  *(undefined4 *)this = param_1;
  uVar1 = move<>(param_2);
  iVar2 = FUN_004fbff0((int)this);
  uVar3 = move<>(iVar2);
                    /* WARNING: Load size is inaccurate */
  FUN_0053fa90(*this,uVar3,uVar1);
  return (undefined4 *)this;
}

