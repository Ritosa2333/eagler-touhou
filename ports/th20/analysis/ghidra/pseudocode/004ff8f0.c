/* Entry: 0x004ff8f0; symbol: FUN_004ff8f0; body bytes: 237 */

void __fastcall FUN_004ff8f0(int param_1)

{
  int iVar1;
  undefined4 local_10;
  undefined4 local_c;
  
  *(uint *)(param_1 + 0x14) = *(uint *)(param_1 + 0x14) & 0xfffffffd;
  for (local_c = 0; local_c < 10; local_c = local_c + 1) {
    iVar1 = FUN_004ba3e0((void *)(param_1 + 0x684),local_c);
    FUN_00486310((void *)(iVar1 + 0xdc));
    iVar1 = FUN_004ba3e0((void *)(param_1 + 0x684),local_c);
    FUN_00486310((void *)(iVar1 + 0xe0));
  }
  for (local_10 = 0; local_10 < 0xc; local_10 = local_10 + 1) {
    iVar1 = FUN_004ba3e0((void *)(param_1 + 0x123c),local_10);
    FUN_00486310((void *)(iVar1 + 0xdc));
    iVar1 = FUN_004ba3e0((void *)(param_1 + 0x123c),local_10);
    FUN_00486310((void *)(iVar1 + 0xe0));
  }
  *(undefined4 *)(param_1 + 0x20e8) = 0;
  return;
}

