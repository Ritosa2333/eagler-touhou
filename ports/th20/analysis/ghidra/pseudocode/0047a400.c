/* Entry: 0x0047a400; symbol: FUN_0047a400; body bytes: 156 */

undefined4 __thiscall
FUN_0047a400(void *this,float param_1,float param_2,float param_3,float param_4)

{
  float fVar1;
  undefined4 local_c;
  
  fVar1 = param_1 - param_3 / DAT_0056c8d0;
                    /* WARNING: Load size is inaccurate */
                    /* WARNING: Load size is inaccurate */
  if ((((*this <= fVar1 && fVar1 != *this) || (param_3 / DAT_0056c8d0 + param_1 < *this)) ||
      (fVar1 = param_2 - param_4 / DAT_0056c8d0,
      *(float *)((int)this + 4) <= fVar1 && fVar1 != *(float *)((int)this + 4))) ||
     (param_4 / DAT_0056c8d0 + param_2 < *(float *)((int)this + 4))) {
    local_c = 1;
  }
  else {
    local_c = 0;
  }
  return local_c;
}

