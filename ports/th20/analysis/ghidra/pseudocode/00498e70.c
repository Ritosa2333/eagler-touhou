/* Entry: 0x00498e70; symbol: FUN_00498e70; body bytes: 196 */

undefined4 __fastcall FUN_00498e70(int param_1)

{
  undefined4 uVar1;
  bool bVar2;
  void *this;
  undefined4 *puVar3;
  uint uVar4;
  int local_c;
  
  local_c = 0;
  while( true ) {
    if (0x3ff < local_c) {
      return 0xffffffff;
    }
    uVar1 = *(undefined4 *)(param_1 + 0x20);
    *(int *)(param_1 + 0x20) = *(int *)(param_1 + 0x20) + 1;
    uVar4 = *(uint *)(param_1 + 0x20) & 0x800003ff;
    if ((int)uVar4 < 0) {
      uVar4 = (uVar4 - 1 | 0xfffffc00) + 1;
    }
    *(uint *)(param_1 + 0x20) = uVar4;
    this = (void *)FUN_00414580((void *)(param_1 + 0x3c),*(int *)(param_1 + 0x20));
    bVar2 = FUN_00449430(this,0);
    if (bVar2) break;
    puVar3 = (undefined4 *)FUN_00414580((void *)(param_1 + 0x3c),*(int *)(param_1 + 0x20));
    bVar2 = FUN_0049bed0(puVar3);
    if (!bVar2) {
      return uVar1;
    }
    local_c = local_c + 1;
  }
  return uVar1;
}

