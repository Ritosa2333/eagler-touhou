/* Entry: 0x00489b90; symbol: FUN_00489b90; body bytes: 43 */

int __cdecl FUN_00489b90(uint param_1)

{
  if (0xa8e83f < param_1) {
    std::_Throw_bad_array_new_length();
  }
  return param_1 * 0x184;
}

