/* Entry: 0x004a1f50; symbol: FUN_004a1f50; body bytes: 43 */

int __cdecl FUN_004a1f50(uint param_1)

{
  if (0x1e1e1e1 < param_1) {
    std::_Throw_bad_array_new_length();
  }
  return param_1 * 0x88;
}

