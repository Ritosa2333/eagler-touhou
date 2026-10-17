/* Entry: 0x004dfb30; symbol: FUN_004dfb30; body bytes: 81 */

void __thiscall FUN_004dfb30(void *this,int param_1)

{
  undefined4 *puVar1;
  undefined4 local_18;
  undefined4 local_14;
  void *local_10;
  undefined4 local_c;
  void *local_8;
  
  local_c = *(undefined4 *)((int)this + 0x9c);
  local_8 = this;
  puVar1 = FUN_0049e080(&local_18,(uint *)"notice",param_1,(undefined4 *)0x0);
  local_14 = *puVar1;
  local_10 = (void *)((int)local_8 + 0x70);
  puVar1 = (undefined4 *)FUN_00414580(local_10,param_1);
  *puVar1 = local_14;
  return;
}

