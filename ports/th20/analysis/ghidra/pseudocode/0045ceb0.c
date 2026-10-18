/* Entry: 0x0045ceb0; symbol: FUN_0045ceb0; body bytes: 92 */

undefined4 __fastcall FUN_0045ceb0(int param_1)

{
  undefined4 *puVar1;
  int iVar2;
  undefined4 local_8;
  
  for (local_8 = 0; local_8 < 200; local_8 = local_8 + 1) {
    puVar1 = (undefined4 *)FUN_00414580((void *)(param_1 + 8),local_8);
    iVar2 = FUN_0044ced0(puVar1);
    if (iVar2 != 0) {
      FUN_0044fd00(iVar2);
    }
  }
  return 0;
}

