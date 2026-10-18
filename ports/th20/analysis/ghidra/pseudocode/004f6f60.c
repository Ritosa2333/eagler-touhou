/* Entry: 0x004f6f60; symbol: FUN_004f6f60; body bytes: 64 */

undefined1 __thiscall FUN_004f6f60(void *this,uint param_1,uint param_2)

{
  undefined1 local_c;
  
                    /* WARNING: Load size is inaccurate */
  if ((param_2 < *(uint *)((int)this + 4)) ||
     ((param_2 <= *(uint *)((int)this + 4) && (param_1 < *this)))) {
    local_c = 0;
  }
  else {
    local_c = 1;
  }
  return local_c;
}

