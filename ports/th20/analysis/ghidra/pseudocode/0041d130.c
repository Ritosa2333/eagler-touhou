/* Entry: 0x0041d130; symbol: FUN_0041d130; body bytes: 210 */

bool __fastcall FUN_0041d130(undefined4 *param_1)

{
  char cVar1;
  uint *puVar2;
  int iVar3;
  uint *puVar4;
  int *unaff_FS_OFFSET;
  bool local_20;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00568060;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  puVar2 = (uint *)FUN_0040ff50(param_1);
  iVar3 = FUN_00411700((int)param_1);
  iVar3 = (int)puVar2 + iVar3 * 2;
  cVar1 = FUN_0041bad0(puVar2,iVar3);
  if (cVar1 == '\0') {
    puVar4 = FUN_0041b920(puVar2,iVar3);
    local_20 = puVar2 != puVar4;
  }
  else if ((iVar3 - (int)puVar2 >> 1 < 3) || (cVar1 = FUN_00419150((short)puVar2[1]), cVar1 == '\0')
          ) {
    local_20 = false;
  }
  else {
    local_20 = true;
  }
  *unaff_FS_OFFSET = local_10;
  return local_20;
}

