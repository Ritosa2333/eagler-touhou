/* Entry: 0x00541ee0; symbol: __Utf8_trailing_byte_count; body bytes: 87 */

/* Library Function - Single Match
    __Utf8_trailing_byte_count
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

undefined4 __cdecl __Utf8_trailing_byte_count(uint *param_1,byte param_2)

{
  uint uVar1;
  undefined4 uStack_8;
  
  if (-1 < (char)param_2) {
    return 0;
  }
  if ((param_2 & 0xe0) == 0xc0) {
    *param_1 = param_2 & 0x1f;
    return 1;
  }
  if ((param_2 & 0xf0) == 0xe0) {
    uVar1 = param_2 & 0xf;
    uStack_8 = 2;
  }
  else {
    if ((param_2 & 0xf8) != 0xf0) {
      return 0x7fffffff;
    }
    uVar1 = param_2 & 7;
    uStack_8 = 3;
  }
  *param_1 = uVar1;
  return uStack_8;
}

