/* Entry: 0x004ed640; symbol: FUN_004ed640; body bytes: 252 */

void __cdecl FUN_004ed640(undefined4 param_1,undefined4 param_2,int *param_3)

{
  int iVar1;
  bool bVar2;
  int iVar3;
  int iVar4;
  int local_44;
  undefined1 local_40 [56];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  iVar1 = *param_3;
  iVar3 = FUN_0040c300(&param_1);
  iVar4 = FUN_0040ff90((int)&param_1);
  FUN_0040c080(local_40,0x38);
  FUN_004f4fd0(local_40,iVar3,iVar4 + iVar3);
  local_44 = 0;
  while( true ) {
    bVar2 = FUN_004f56c0((int)local_40);
    if ((bVar2) || ((local_44 == iVar1 && (-1 < iVar1)))) goto LAB_004ed71e;
    iVar3 = FUN_004859d0((int)local_40);
    if (0x7fffffff - iVar3 < local_44) break;
    local_44 = local_44 + iVar3;
    if ((iVar1 < local_44) && (-1 < iVar1)) {
      local_44 = local_44 - iVar3;
      goto LAB_004ed71e;
    }
    FUN_004f5810(local_40);
  }
  if (iVar1 < 0) {
    *param_3 = 0x7fffffff;
    goto LAB_004ed72e;
  }
LAB_004ed71e:
  *param_3 = local_44;
  FUN_00464250((int)local_40);
LAB_004ed72e:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

