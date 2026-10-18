/* Entry: 0x0049c2e0; symbol: FUN_0049c2e0; body bytes: 85 */

void __thiscall FUN_0049c2e0(void *this,uint param_1,void *param_2)

{
  undefined4 *puVar1;
  void *this_00;
  undefined4 uVar2;
  undefined4 local_14;
  void *local_10;
  void *local_c;
  undefined4 local_8;
  
  local_c = this;
  if (param_2 == (void *)0x0) {
    local_8 = 0;
  }
  else {
    puVar1 = FUN_00498f90(param_2,&local_14);
    local_8 = FUN_0040c300(puVar1);
  }
  local_10 = (void *)((int)local_c + 0x54);
  uVar2 = local_8;
  this_00 = (void *)FUN_00484980(local_10,param_1);
  FUN_004117a0(this_00,uVar2);
  return;
}

