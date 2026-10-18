/* Entry: 0x00501650; symbol: FUN_00501650; body bytes: 278 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __fastcall FUN_00501650(int param_1)

{
  int iVar1;
  float *pfVar2;
  undefined4 local_10;
  undefined4 local_c;
  
  *(undefined1 *)(param_1 + 0x114) = 1;
  *(uint *)(param_1 + 0x3c) = *(uint *)(param_1 + 0x3c) & 0xfffffffe;
  iVar1 = FUN_0040ff90(*(int *)(param_1 + 0x120));
  pfVar2 = (float *)FUN_00479020(iVar1);
  if (DAT_005732d0 < *pfVar2 || DAT_005732d0 == *pfVar2) {
    iVar1 = FUN_0040ff90(*(int *)(param_1 + 0x120));
    pfVar2 = (float *)FUN_00479020(iVar1);
    if (*pfVar2 <= DAT_0056ef28) {
      local_c = DAT_0056e0f8 / DAT_0056c8d0;
    }
    else {
      local_c = (DAT_0056e0f0 / _DAT_00570db0) * DAT_0056f00c + DAT_0056e0f8 / DAT_0056c8d0;
    }
    local_10 = local_c;
  }
  else {
    local_10 = (float)((uint)(DAT_0056e0f0 / _DAT_00570db0) ^ _DAT_0056d7d0) * DAT_0056f00c +
               DAT_0056e0f8 / DAT_0056c8d0;
  }
  FUN_0047a560((void *)(param_1 + 0x50),local_10);
  return 0;
}

