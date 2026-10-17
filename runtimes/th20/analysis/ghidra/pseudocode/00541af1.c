/* Entry: 0x00541af1; symbol: FUN_00541af1; body bytes: 45 */

void __cdecl FUN_00541af1(undefined4 param_1)

{
  code *pcVar1;
  undefined4 *puVar2;
  
  puVar2 = (undefined4 *)FUN_0054a550(8);
  if (puVar2 != (undefined4 *)0x0) {
    *puVar2 = DAT_005e4e24;
    puVar2[1] = param_1;
    DAT_005e4e24 = puVar2;
    return;
  }
  __scrt_throw_std_bad_alloc();
  pcVar1 = (code *)swi(3);
  (*pcVar1)();
  return;
}

