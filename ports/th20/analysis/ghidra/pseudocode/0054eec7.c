/* Entry: 0x0054eec7; symbol: FUN_0054eec7; body bytes: 70 */

ushort __fastcall FUN_0054eec7(undefined4 *param_1)

{
  ushort uVar1;
  uint uVar2;
  uint uVar3;
  
  uVar2 = param_1[4] + 1;
  uVar3 = param_1[5] + (uint)(0xfffffffe < (uint)param_1[4]);
  param_1[4] = uVar2;
  param_1[5] = uVar3;
  if (((param_1[2] != 0 || param_1[3] != 0) && ((uint)param_1[3] <= uVar3)) &&
     (((uint)param_1[3] < uVar3 || ((uint)param_1[2] < uVar2)))) {
    return 0;
  }
  uVar1 = __crt_stdio_input::string_input_adapter<wchar_t>::get
                    ((string_input_adapter<wchar_t> *)*param_1);
  if (uVar1 == 0xffff) {
    uVar1 = 0;
  }
  return uVar1;
}

