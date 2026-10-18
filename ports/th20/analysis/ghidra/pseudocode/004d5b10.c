/* Entry: 0x004d5b10; symbol: FUN_004d5b10; body bytes: 150 */

undefined4 __cdecl FUN_004d5b10(void *param_1)

{
  uint uVar1;
  undefined4 uVar2;
  float10 fVar3;
  
  uVar1 = FUN_00424030(DAT_005ba828);
  if (uVar1 == 0) {
    if ((*(uint *)(DAT_005ba828 + 0xe8) >> 0xb & 1) == 0) {
      uVar1 = FUN_00424060(DAT_005ba828);
      if (uVar1 == 0) {
        uVar2 = FUN_004cf680(param_1);
      }
      else {
        fVar3 = FUN_004292e0((float *)&DAT_005aefe4);
        FUN_004292a0(&DAT_005aefe4,0);
        uVar2 = FUN_004cf680(param_1);
        FUN_004292a0(&DAT_005aefe4,(float)fVar3);
      }
    }
    else {
      uVar2 = 1;
    }
  }
  else {
    uVar2 = 1;
  }
  return uVar2;
}

