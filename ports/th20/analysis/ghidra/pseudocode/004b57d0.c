/* Entry: 0x004b57d0; symbol: FUN_004b57d0; body bytes: 72 */

void __fastcall FUN_004b57d0(int param_1)

{
  FUN_00479040((void *)(param_1 + 0x118));
  *(uint *)(param_1 + 0x1a4) = *(uint *)(param_1 + 0x1a4) & 0xffffe7ff | 0x800;
  FUN_00423520((void *)(param_1 + 0x1a8),0);
  return;
}

