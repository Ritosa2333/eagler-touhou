/* Entry: 0x00423ea0; symbol: FUN_00423ea0; body bytes: 49 */

uint __thiscall FUN_00423ea0(void *this,uint param_1)

{
  undefined4 local_8;
  
  if (param_1 == 0) {
    local_8 = 0;
  }
  else {
    local_8 = FUN_00423ee0((int)this);
    local_8 = local_8 % param_1;
  }
  return local_8;
}

