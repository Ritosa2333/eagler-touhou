/* Entry: 0x0040ddd0; symbol: FUN_0040ddd0; body bytes: 42 */

undefined4 * __thiscall FUN_0040ddd0(void *this,char *param_1)

{
  int iVar1;
  
  *(char **)this = param_1;
  iVar1 = FUN_00411180(param_1);
  *(int *)((int)this + 4) = iVar1;
  return (undefined4 *)this;
}

