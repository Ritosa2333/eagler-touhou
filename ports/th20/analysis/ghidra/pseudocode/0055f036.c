/* Entry: 0x0055f036; symbol: FUN_0055f036; body bytes: 80 */

undefined4 __cdecl FUN_0055f036(int *param_1,byte param_2,uint param_3,byte param_4)

{
  undefined4 uVar1;
  int local_14;
  int *local_10;
  int local_c;
  char local_8;
  
  FUN_0054c44c(&local_14,param_1);
  if (((*(byte *)(local_c + 0x19 + (uint)param_2) & param_4) == 0) &&
     ((param_3 == 0 || ((param_3 & *(ushort *)(*local_10 + (uint)param_2 * 2)) == 0)))) {
    uVar1 = 0;
  }
  else {
    uVar1 = 1;
  }
  if (local_8 != '\0') {
    *(uint *)(local_14 + 0x350) = *(uint *)(local_14 + 0x350) & 0xfffffffd;
  }
  return uVar1;
}

