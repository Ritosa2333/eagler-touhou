/* Entry: 0x0045dbf0; symbol: FUN_0045dbf0; body bytes: 123 */

void __thiscall FUN_0045dbf0(void *this,undefined4 param_1)

{
  void *this_00;
  undefined4 uVar1;
  uint local_18;
  void *local_14;
  void *local_10;
  undefined4 local_c;
  void *local_8;
  
  local_10 = (void *)((int)this + 0x1e0);
  local_8 = this;
  FUN_004142a0(&local_c,param_1);
  FUN_0045dd20(local_10,&local_c);
  local_14 = (void *)((int)local_8 + 0x1e0);
  uVar1 = 0x80ffff80;
  this_00 = (void *)FUN_00414580(local_14,0);
  FUN_0045d650(this_00,uVar1);
  local_18 = DAT_0056cd94;
  FUN_0045dd00((void *)((int)local_8 + 0x2d0),&local_18);
  return;
}

