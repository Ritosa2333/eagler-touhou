/* Entry: 0x004fbdb0; symbol: FUN_004fbdb0; body bytes: 110 */

void __thiscall FUN_004fbdb0(void *this,char *param_1,int param_2)

{
  mbstate_t local_14 [2];
  wchar_t local_c [2];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_14[0] = 0;
  local_14[1] = 0;
  __Mbrtowc(local_c,param_1,param_2 - (int)param_1,local_14,(_Cvtvec *)this);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

