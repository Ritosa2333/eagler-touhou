/* Entry: 0x005559a6; symbol: FUN_005559a6; body bytes: 88 */

byte * FUN_005559a6(void)

{
  bool bVar1;
  byte bVar2;
  int iVar3;
  byte *pbVar4;
  byte *pbVar5;
  
  ___acrt_initialize_multibyte();
  pbVar5 = &DAT_005e55d8;
  if (DAT_005e5988 != (byte *)0x0) {
    pbVar5 = DAT_005e5988;
  }
  bVar1 = false;
  do {
    bVar2 = *pbVar5;
    if (bVar2 < 0x21) {
      if (bVar2 == 0) {
        return pbVar5;
      }
      if (!bVar1) {
        do {
          if (0x20 < bVar2) {
            return pbVar5;
          }
          pbVar5 = pbVar5 + 1;
          bVar2 = *pbVar5;
        } while (bVar2 != 0);
        return pbVar5;
      }
    }
    if (bVar2 == 0x22) {
      bVar1 = !bVar1;
    }
    iVar3 = FUN_0055f086(bVar2);
    pbVar4 = pbVar5 + 1;
    if (iVar3 == 0) {
      pbVar4 = pbVar5;
    }
    pbVar5 = pbVar4 + 1;
  } while( true );
}

