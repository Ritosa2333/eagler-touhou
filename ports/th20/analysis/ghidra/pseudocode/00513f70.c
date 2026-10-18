/* Entry: 0x00513f70; symbol: FUN_00513f70; body bytes: 256 */

uint __fastcall FUN_00513f70(int param_1)

{
  bool bVar1;
  char cVar2;
  int iVar3;
  undefined4 *puVar4;
  uint local_c;
  int local_8;
  
  cVar2 = FUN_00485a40(param_1);
  if ((cVar2 == '\0') || (iVar3 = FUN_004859f0(param_1), iVar3 == 0)) {
    local_8 = -1;
    iVar3 = FUN_00464080(0);
    iVar3 = FUN_00513ea0(iVar3);
    if (-1 < iVar3) {
      iVar3 = FUN_00464080(0);
      local_8 = FUN_00513ea0(iVar3);
    }
    iVar3 = FUN_00464080(0);
    iVar3 = FUN_00513de0(iVar3);
    bVar1 = local_8 < iVar3;
    if (bVar1) {
      iVar3 = FUN_00464080(0);
      local_8 = FUN_00513de0(iVar3);
    }
    local_c = (uint)bVar1;
    iVar3 = FUN_00464080(0);
    iVar3 = FUN_00513ee0(iVar3);
    if (local_8 < iVar3) {
      iVar3 = FUN_00464080(0);
      local_8 = FUN_00513ee0(iVar3);
      local_c = 2;
    }
    iVar3 = FUN_00464080(0);
    iVar3 = FUN_00513e20(iVar3);
    if (local_8 < iVar3) {
      local_c = 3;
    }
  }
  else {
    puVar4 = (undefined4 *)FUN_004859f0(param_1);
    iVar3 = FUN_0040c300(puVar4);
    local_c = *(uint *)(iVar3 + 0x2c);
  }
  return local_c;
}

