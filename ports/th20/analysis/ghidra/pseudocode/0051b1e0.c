/* Entry: 0x0051b1e0; symbol: FUN_0051b1e0; body bytes: 73 */

void __thiscall FUN_0051b1e0(void *this,uint param_1,bool param_2)

{
  undefined4 local_c;
  
  if (*(int *)((int)this + 0x38) == 0) {
    local_c = 4;
  }
  else {
    local_c = 0;
  }
  std::ios_base::clear((ios_base *)this,param_1 | local_c,param_2);
  return;
}

