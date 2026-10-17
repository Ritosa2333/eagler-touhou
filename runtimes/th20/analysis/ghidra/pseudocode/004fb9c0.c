/* Entry: 0x004fb9c0; symbol: FUN_004fb9c0; body bytes: 191 */

void __cdecl FUN_004fb9c0(uint param_1)

{
  bool bVar1;
  char *pcVar2;
  undefined1 local_20 [4];
  undefined4 local_1c;
  undefined4 *local_18;
  int *local_14;
  int local_10;
  int local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  bVar1 = FUN_00410390((int)&stack0x00000008);
  if (!bVar1) {
    local_10 = 0;
    FUN_0040c080(&local_c,4);
    FUN_004fed50(&stack0x00000008,&local_c);
    while (pcVar2 = (char *)FUN_0040c300(&local_c), (uint)(int)*pcVar2 < param_1) {
      pcVar2 = (char *)FUN_0040c300(&local_c);
      param_1 = param_1 - (int)*pcVar2;
      local_10 = local_10 + 1;
      local_18 = FUN_004f58c0(&local_c,&local_1c,1);
      local_14 = (int *)FUN_004fefc0(&stack0x00000008,local_20);
      bVar1 = FUN_00449400(local_18,local_14);
      if (!bVar1) {
        FUN_00461860(&local_c);
      }
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

