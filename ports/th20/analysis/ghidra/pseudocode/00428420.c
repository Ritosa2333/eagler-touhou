/* Entry: 0x00428420; symbol: FUN_00428420; body bytes: 320 */

void __thiscall FUN_00428420(void *this,undefined1 (*param_1) [16])

{
  int iVar1;
  undefined1 (*local_90) [16];
  int local_8c;
  char local_88 [128];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_8c = 0;
  local_90 = FUN_005448c0(param_1,0x2f);
  if (local_90 == (undefined1 (*) [16])0x0) {
    local_90 = FUN_005448c0(param_1,0x5c);
  }
  if (local_90 == (undefined1 (*) [16])0x0) {
    FUN_00548610(local_88,0x80,(int)param_1);
  }
  else {
    FUN_00548610(local_88,0x80,(int)(*local_90 + 1));
  }
  while ((*(char *)(*(int *)((int)this + 0x1890) + local_8c * 0x34) != '\0' &&
         (iVar1 = _strcmp((char *)(*(int *)((int)this + 0x1890) + local_8c * 0x34),local_88),
         iVar1 != 0))) {
    local_8c = local_8c + 1;
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

