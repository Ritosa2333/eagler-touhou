/* Entry: 0x004d4660; symbol: FUN_004d4660; body bytes: 123 */

bool __fastcall FUN_004d4660(int param_1)

{
  bool bVar1;
  void *pvVar2;
  
  pvVar2 = (void *)FUN_0047c680((void *)(param_1 + 0x90),0xb);
  FUN_00429420(pvVar2);
  pvVar2 = (void *)FUN_0047c680((void *)(param_1 + 0x90),0xb);
  bVar1 = FUN_00423590(pvVar2,0);
  if (bVar1) {
    *(uint *)(param_1 + 0x698) = *(uint *)(param_1 + 0x698) & 0xfffffeff;
  }
  return bVar1;
}

