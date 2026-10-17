/* Entry: 0x004de1f0; symbol: FUN_004de1f0; body bytes: 359 */

int FUN_004de1f0(void)

{
  int *piVar1;
  int iVar2;
  
  FUN_004dc0f0(0x5c4d40);
  DAT_005c5854 = 0;
  piVar1 = FUN_004127b0(FUN_004dc510);
  FUN_00412d10(piVar1,&DAT_005c4d40);
  FUN_00412dc0(piVar1,FUN_004dd600);
  FUN_00412d80((int)piVar1);
  iVar2 = FUN_00411f80(DAT_005b66d8,piVar1,1);
  if (iVar2 == 0) {
    FUN_00412360(1,FUN_004dc5f0,&DAT_005c4d40);
    FUN_00412360(0xe,FUN_004dc8b0,&DAT_005c4d40);
    FUN_00412360(0xf,FUN_004d9120,&DAT_005c4d40);
    FUN_00412360(0x19,FUN_004dd1c0,&DAT_005c4d40);
    FUN_00412360(0x1a,FUN_004d8e90,&DAT_005c4d40);
    FUN_00412360(0x2f,FUN_004dd2e0,&DAT_005c4d40);
    FUN_00412360(0x30,FUN_004d8f80,&DAT_005c4d40);
    FUN_00412360(0x42,FUN_004dd400,&DAT_005c4d40);
    FUN_00412360(0x43,FUN_004d9040,&DAT_005c4d40);
    FUN_00412360(0x6e,FUN_004dc7c0,&DAT_005c4d40);
    piVar1 = (int *)FUN_00412730(0x5c4d40);
    (**(code **)(*piVar1 + 0x48))(piVar1,0,0,0,&DAT_005c4ee4);
    iVar2 = 0;
  }
  return iVar2;
}

