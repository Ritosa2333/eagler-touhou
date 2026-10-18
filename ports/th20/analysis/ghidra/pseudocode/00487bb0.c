/* Entry: 0x00487bb0; symbol: FUN_00487bb0; body bytes: 123 */

void __fastcall FUN_00487bb0(int param_1)

{
  bool bVar1;
  undefined3 extraout_var;
  int iVar2;
  
  bVar1 = FUN_004887a0(param_1);
  if (bVar1) {
    bVar1 = FUN_004887d0(param_1);
    if (CONCAT31(extraout_var,bVar1) == 0) {
      iVar2 = FUN_00424010(*(int *)(param_1 + 0xc0));
      bVar1 = FUN_00478130(iVar2);
      if (bVar1) {
        *(uint *)(param_1 + 0x78) = *(uint *)(param_1 + 0x78) | 0x20;
      }
    }
    else {
      *(undefined4 *)(param_1 + 0x7c) = 0;
      *(uint *)(param_1 + 0x78) = *(uint *)(param_1 + 0x78) & 0xfffffffd;
      *(uint *)(param_1 + 0x78) = *(uint *)(param_1 + 0x78) & 0xffffffdf;
    }
  }
  return;
}

