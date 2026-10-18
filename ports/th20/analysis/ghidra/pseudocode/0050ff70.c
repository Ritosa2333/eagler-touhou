/* Entry: 0x0050ff70; symbol: FUN_0050ff70; body bytes: 413 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __fastcall FUN_0050ff70(int param_1)

{
  bool bVar1;
  int iVar2;
  float10 fVar3;
  int local_14;
  int local_10;
  int local_8;
  
  FUN_004776a0(DAT_005c0028,*(undefined4 *)(param_1 + 0xf90));
  local_8 = param_1 + 0x600;
  for (local_10 = 0; local_10 < 0xd; local_10 = local_10 + 1) {
    if (*(char *)(local_8 + 0x38) != '\0') {
      fVar3 = FUN_004292e0((float *)&DAT_005aefe4);
      *(float *)(local_8 + 0x10) =
           *(float *)(local_8 + 0x10) - (float)fVar3 * *(float *)(local_8 + 0x18);
      *(float *)(local_8 + 0x18) = *(float *)(local_8 + 0x18) * _DAT_0057178c;
      FUN_00423540((undefined4 *)(local_8 + 0x20));
      bVar1 = FUN_00461070((void *)(local_8 + 0x20),0x3c);
      if (bVar1) {
        *(undefined1 *)(local_8 + 0x38) = 0;
      }
    }
    local_8 = local_8 + 0x44;
  }
  for (local_14 = 0; local_14 < 5; local_14 = local_14 + 1) {
    if (*(char *)(local_8 + 0x38) != '\0') {
      FUN_00423540((undefined4 *)(local_8 + 0x20));
      bVar1 = FUN_00461070((void *)(local_8 + 0x20),0x3c);
      if (bVar1) {
        iVar2 = (*(uint *)(local_8 + 0x1c) >> 0x18) - 4;
        if (iVar2 < 1) {
          *(undefined1 *)(local_8 + 0x38) = 0;
        }
        else {
          *(uint *)(local_8 + 0x1c) = iVar2 * 0x1000000 | *(uint *)(local_8 + 0x1c) & 0xffffff;
        }
      }
    }
    local_8 = local_8 + 0x44;
  }
  return 1;
}

