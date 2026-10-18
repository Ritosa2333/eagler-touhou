/* Entry: 0x0041b8a0; symbol: FUN_0041b8a0; body bytes: 55 */

void __thiscall FUN_0041b8a0(void *this,int param_1)

{
  undefined4 *puVar1;
  undefined1 *puVar2;
  undefined1 local_5;
  
  *(int *)((int)this + 0x14) = param_1;
  local_5 = 0;
  puVar2 = &local_5;
  puVar1 = Myptr((undefined4 *)((int)this + 4));
  FUN_0040fe90((undefined1 *)((int)puVar1 + param_1),puVar2);
  return;
}

