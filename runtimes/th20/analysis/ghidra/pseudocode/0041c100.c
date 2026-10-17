/* Entry: 0x0041c100; symbol: FUN_0041c100; body bytes: 148 */

undefined4 __cdecl FUN_0041c100(undefined1 (*param_1) [32],undefined1 (*param_2) [32],uint param_3)

{
  char cVar1;
  undefined4 local_18;
  undefined1 (*local_14) [32];
  undefined1 (*local_10) [32];
  uint local_c;
  
  cVar1 = FUN_00411170();
  if (cVar1 == '\0') {
    local_18 = FUN_0041f260(param_1,param_2,param_3);
  }
  else {
    local_14 = param_2;
    local_10 = param_1;
    local_18 = 0;
    for (local_c = param_3; local_c != 0; local_c = local_c - 1) {
      if (*(ushort *)*local_10 != *(ushort *)*local_14) {
        if (*(ushort *)*local_10 < *(ushort *)*local_14) {
          return 0xffffffff;
        }
        return 1;
      }
      local_10 = (undefined1 (*) [32])(*local_10 + 2);
      local_14 = (undefined1 (*) [32])(*local_14 + 2);
    }
  }
  return local_18;
}

