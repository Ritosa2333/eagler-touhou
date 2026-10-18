/* Entry: 0x00549573; symbol: FUN_00549573; body bytes: 98 */

void __cdecl FUN_00549573(byte *param_1,int *param_2)

{
  byte bVar1;
  int *piVar2;
  byte bVar3;
  byte *pbVar4;
  bool bVar5;
  
  piVar2 = (int *)*param_2;
  bVar3 = *param_1;
  if (*(char *)((uint)bVar3 + piVar2[0x25]) != 'e') {
    do {
      param_1 = param_1 + 1;
      bVar3 = *param_1;
    } while ((*(byte *)(*piVar2 + (uint)bVar3 * 2) & 4) != 0);
  }
  bVar5 = *(char *)((uint)bVar3 + piVar2[0x25]) == 'x';
  if (bVar5) {
    bVar3 = param_1[2];
  }
  pbVar4 = param_1 + 2;
  if (!bVar5) {
    pbVar4 = param_1;
  }
  *pbVar4 = **(byte **)piVar2[0x22];
  do {
    pbVar4 = pbVar4 + 1;
    bVar1 = *pbVar4;
    *pbVar4 = bVar3;
    bVar5 = bVar3 != 0;
    bVar3 = bVar1;
  } while (bVar5);
  return;
}

