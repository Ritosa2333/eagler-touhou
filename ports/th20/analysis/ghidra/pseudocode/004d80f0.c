/* Entry: 0x004d80f0; symbol: FUN_004d80f0; body bytes: 110 */

undefined4 __fastcall FUN_004d80f0(int param_1)

{
  if ((*(uint *)(param_1 + 4) >> 1 & 1) != 0) {
    FUN_004dbd70(0x5c4d40);
    DAT_005b8858 = 1;
    (**(code **)(*DAT_005c0698 + 4))();
    DAT_005c5888 = DAT_005c5888 & 0xfffffdff;
    FUN_004be250(&DAT_005c4d40,4);
    *(uint *)(param_1 + 4) = *(uint *)(param_1 + 4) & 0xfffffffd;
  }
  return 1;
}

