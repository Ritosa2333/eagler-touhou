/* Entry: 0x0053e560; symbol: FUN_0053e560; body bytes: 82 */

void __fastcall FUN_0053e560(int param_1)

{
  undefined4 *puVar1;
  int iVar2;
  undefined4 local_8;
  
  local_8 = (undefined4 *)FUN_0040ff90(param_1 + 0x5c);
  while (local_8 != (undefined4 *)0x0) {
    puVar1 = (undefined4 *)FUN_0040ff90((int)local_8);
    iVar2 = FUN_0040c300(local_8);
    *(undefined4 *)(iVar2 + 8) = 0xffffffff;
    iVar2 = FUN_0040c300(local_8);
    *(undefined4 *)(iVar2 + 4) = 0xffffffff;
    local_8 = puVar1;
  }
  return;
}

