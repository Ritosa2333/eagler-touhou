/* Entry: 0x004f8af0; symbol: FUN_004f8af0; body bytes: 145 */

undefined4 __fastcall FUN_004f8af0(int param_1)

{
  int iVar1;
  undefined4 *puVar2;
  
  iVar1 = FUN_00411700(param_1);
  if (iVar1 != 2) {
    FUN_0044f3d0(DAT_005c0028,0xc,*(int *)(param_1 + 0x14854));
    puVar2 = (undefined4 *)FUN_00460850(param_1);
    *(undefined4 *)(param_1 + 0x5e4) = *puVar2;
    *(undefined4 *)(param_1 + 0x5e8) = puVar2[1];
    *(undefined4 *)(param_1 + 0x5ec) = puVar2[2];
    *(byte *)(param_1 + 0x4cb) = *(byte *)(param_1 + 0x4cb) & 0xfc | 1;
    FUN_00443880(DAT_005c0028,(void *)(param_1 + 0x28));
    FUN_004455c0((int)DAT_005c0028);
  }
  return 1;
}

