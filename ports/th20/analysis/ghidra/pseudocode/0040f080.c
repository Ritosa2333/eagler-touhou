/* Entry: 0x0040f080; symbol: FUN_0040f080; body bytes: 52 */

void __thiscall FUN_0040f080(void *this,int param_1)

{
  undefined4 *puVar1;
  undefined1 *puVar2;
  undefined1 local_5;
  
  *(int *)((int)this + 0x10) = param_1;
  local_5 = 0;
  puVar2 = &local_5;
  puVar1 = Myptr((undefined4 *)this);
  FUN_0040fe90((undefined1 *)((int)puVar1 + param_1),puVar2);
  return;
}

