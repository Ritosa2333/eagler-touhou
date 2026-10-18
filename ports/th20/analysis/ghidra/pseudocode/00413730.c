/* Entry: 0x00413730; symbol: FUN_00413730; body bytes: 40 */

int __cdecl FUN_00413730(uint param_1)

{
  if (0x5555555 < param_1) {
    std::_Throw_bad_array_new_length();
  }
  return param_1 * 0x30;
}

