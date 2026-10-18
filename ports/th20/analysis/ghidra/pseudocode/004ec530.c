/* Entry: 0x004ec530; symbol: FUN_004ec530; body bytes: 100 */

void __fastcall FUN_004ec530(int *param_1)

{
  _Lockit local_c [4];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  if (*param_1 == 0) {
    std::_Lockit::_Lockit(local_c,0);
    if (*param_1 == 0) {
      DAT_005e4e00 = DAT_005e4e00 + 1;
      *param_1 = DAT_005e4e00;
    }
    FUN_005419ac((int *)local_c);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

