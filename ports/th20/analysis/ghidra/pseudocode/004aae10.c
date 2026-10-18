/* Entry: 0x004aae10; symbol: FUN_004aae10; body bytes: 151 */

undefined4 __thiscall FUN_004aae10(void *this,uint param_1)

{
  int iVar1;
  void *this_00;
  undefined4 *puVar2;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  
  if (param_1 == 7) {
    local_14 = FUN_004aadf0(DAT_005c069c);
  }
  else {
    if (param_1 == 0) {
      iVar1 = FUN_0040c300(*(undefined4 **)((int)this + 0x130));
      local_10 = FUN_004859b0(iVar1);
    }
    else {
      if (param_1 == 1) {
        this_00 = (void *)FUN_0041cad0(*(int *)((int)this + 0x130));
        local_c = FUN_0045d100(this_00,0);
      }
      else {
        puVar2 = (undefined4 *)FUN_004aa130((void *)((int)this + 0xe4),param_1);
        local_c = *puVar2;
      }
      local_10 = local_c;
    }
    local_14 = local_10;
  }
  return local_14;
}

