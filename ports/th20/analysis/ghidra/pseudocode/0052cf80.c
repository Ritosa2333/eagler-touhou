/* Entry: 0x0052cf80; symbol: FUN_0052cf80; body bytes: 83 */

void __thiscall FUN_0052cf80(void *this,int param_1)

{
  undefined4 *puVar1;
  undefined4 local_18;
  undefined4 local_14;
  void *local_10;
  undefined4 local_c;
  void *local_8;
  
  local_c = *(undefined4 *)((int)this + 0x10);
  local_8 = this;
  puVar1 = FUN_00450c70(&local_18,(uint *)"title",param_1,-1,(undefined4 *)0x0);
  local_14 = *puVar1;
  local_10 = (void *)((int)local_8 + 0x164);
  puVar1 = (undefined4 *)FUN_00414580(local_10,param_1);
  *puVar1 = local_14;
  return;
}

