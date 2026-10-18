/* Entry: 0x00506210; symbol: FUN_00506210; body bytes: 214 */

undefined4 * __fastcall FUN_00506210(int param_1)

{
  undefined4 uVar1;
  undefined4 *puVar2;
  int iVar3;
  
  puVar2 = (undefined4 *)FUN_004859f0(param_1 + 0x12448);
  if (puVar2 == (undefined4 *)0x0) {
    puVar2 = FUN_005041b0();
    FUN_0044a670(puVar2,puVar2);
    FUN_0044a6b0((void *)(param_1 + 0x12430),puVar2);
    puVar2[5] = *(uint *)(param_1 + 0x12460) | 0x1000000;
    FUN_00506d80(puVar2,0);
    FUN_00505fc0(param_1);
  }
  else {
    FUN_00411ce0((int)puVar2);
    FUN_0044a6b0((void *)(param_1 + 0x12430),puVar2);
    uVar1 = *(undefined4 *)(param_1 + 0x12460);
    iVar3 = FUN_0040c300(puVar2);
    *(undefined4 *)(iVar3 + 0x14) = uVar1;
    FUN_00505fc0(param_1);
    puVar2 = (undefined4 *)FUN_0040c300(puVar2);
  }
  return puVar2;
}

