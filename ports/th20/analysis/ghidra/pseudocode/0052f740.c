/* Entry: 0x0052f740; symbol: FUN_0052f740; body bytes: 129 */

undefined4 __thiscall FUN_0052f740(void *this,uint param_1)

{
  uint uVar1;
  undefined4 *puVar2;
  _Iterator_base12 *p_Var3;
  undefined1 auStack_50 [4];
  undefined4 uStack_4c;
  undefined1 local_40 [12];
  undefined1 local_34 [12];
  undefined1 local_28 [12];
  undefined4 local_1c;
  void *local_18;
  void *local_14;
  void *local_10;
  void *local_c;
  void *local_8;
  
  local_8 = this;
  uVar1 = FUN_0052f8e0((int)this);
  if (param_1 < uVar1) {
    local_c = local_8;
    uStack_4c = 0x52f76d;
    puVar2 = (undefined4 *)FUN_0052dce0(local_8,param_1);
    local_1c = *puVar2;
    local_18 = local_8;
    local_10 = local_8;
    uStack_4c = 0x52f78a;
    local_14 = FUN_0052efe0(local_8,local_28);
    p_Var3 = (_Iterator_base12 *)FUN_0052de60(local_14,local_34,param_1);
    _String_const_iterator<>(auStack_50,p_Var3);
    FUN_0052f420(local_40);
  }
  else {
    local_1c = 0;
  }
  return local_1c;
}

