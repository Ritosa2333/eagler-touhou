/* Entry: 0x004bec70; symbol: FUN_004bec70; body bytes: 175 */

undefined4 * __cdecl FUN_004bec70(undefined4 param_1)

{
  undefined4 *puVar1;
  int *piVar2;
  
  puVar1 = FUN_004b9b10();
  DAT_005b8858 = 0;
  piVar2 = (int *)FUN_00412730(0x5c4d40);
  (**(code **)(*piVar2 + 0x14))(piVar2);
  DAT_005ba828 = puVar1;
  puVar1[0x42] = param_1;
  puVar1[0x3a] = puVar1[0x3a] | 4;
  piVar2 = (int *)FUN_00464230(0);
  (**(code **)(*piVar2 + 8))();
  piVar2 = (int *)FUN_0051b960();
  (**(code **)(*piVar2 + 8))();
  FUN_004b99f0(FUN_004bcca0);
  return puVar1;
}

