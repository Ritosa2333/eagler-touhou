/* Entry: 0x00552818; symbol: FUN_00552818; body bytes: 67 */

int __cdecl FUN_00552818(uint param_1)

{
  uint uVar1;
  int iVar2;
  bool bVar3;
  
  uVar1 = param_1 & 0x80000003;
  bVar3 = uVar1 == 0;
  if ((int)uVar1 < 0) {
    bVar3 = (uVar1 - 1 | 0xfffffffc) == 0xffffffff;
  }
  if (bVar3) {
    if ((int)param_1 % 100 != 0) {
      return CONCAT31((int3)((uint)((int)param_1 / 100) >> 8),1);
    }
  }
  iVar2 = (int)(param_1 + 0x76c) % 400;
  return CONCAT31((int3)((uint)-iVar2 >> 8),-(iVar2 != 0)) + 1;
}

