/* Entry: 0x0041e050; symbol: FUN_0041e050; body bytes: 1819 */

void __thiscall FUN_0041e050(void *this,int param_1)

{
  int iVar1;
  int local_10;
  int local_c;
  
  iVar1 = FUN_0041cc70(0x5b6758);
  if ((iVar1 == 9) || (iVar1 = FUN_0041cc70(0x5b6758), iVar1 == 8)) {
    DAT_005b8800 = DAT_005b8808;
    DAT_005b8804 = DAT_005b880c;
    if (param_1 != 0) {
      if ((DAT_005b8808 < 0xa00) || (DAT_005b880c < 0x780)) {
        if ((DAT_005b8808 < 0x780) || (DAT_005b880c < 0x5a0)) {
          if ((DAT_005b8808 < 0x500) || (DAT_005b880c < 0x3c0)) {
            if ((DAT_005b8808 < 0x3c0) || (DAT_005b880c < 0x2d0)) {
              DAT_005c4f8a = 0;
              DAT_005b8818 = DAT_0056c8cc;
            }
            else {
              DAT_005b8818 = DAT_0056cd68;
              DAT_005c4f8a = 1;
            }
          }
          else {
            DAT_005b8818 = DAT_0056c8d0;
            DAT_005c4f8a = 2;
          }
        }
        else {
          DAT_005b8818 = DAT_0056c8d0;
          DAT_005c4f8a = 3;
        }
      }
      else {
        DAT_005b8818 = DAT_0056c8d0;
        DAT_005c4f8a = 4;
      }
    }
    DAT_005b87f8 = (int)(DAT_0056cdac * DAT_005b8818);
    DAT_005b87fc = (int)(DAT_0056cda8 * DAT_005b8818);
    iVar1 = FUN_0041cc70(0x5b6758);
    if (iVar1 == 9) {
      if ((float)DAT_005b8800 / (float)DAT_005b8804 < (float)DAT_005b87f8 / (float)DAT_005b87fc) {
        DAT_005b8810 = DAT_005b87f8;
        DAT_005b8814 = (int)((float)DAT_005b87f8 * ((float)DAT_005b8804 / (float)DAT_005b8800));
      }
      else {
        DAT_005b8810 = (int)((float)DAT_005b87fc * ((float)DAT_005b8800 / (float)DAT_005b8804));
        DAT_005b8814 = DAT_005b87fc;
      }
    }
    else if ((float)DAT_005b8800 / (float)DAT_005b8804 < (float)DAT_005b87f8 / (float)DAT_005b87fc)
    {
      for (local_10 = DAT_005b87f8; DAT_005b87f8 / 2 + local_10 < DAT_005b8800;
          local_10 = DAT_005b87f8 / 2 + local_10) {
      }
      DAT_005b8810 = (int)((float)DAT_005b87fc * ((float)DAT_005b8800 / (float)local_10));
      DAT_005b8814 = (int)((float)DAT_005b8814 * ((float)DAT_005b8804 / (float)DAT_005b8800));
    }
    else {
      for (local_c = DAT_005b87fc; DAT_005b87fc / 2 + local_c < DAT_005b8804;
          local_c = DAT_005b87fc / 2 + local_c) {
      }
      DAT_005b8814 = (int)((float)DAT_005b87fc * ((float)DAT_005b8804 / (float)local_c));
      DAT_005b8810 = (int)((float)DAT_005b8814 * ((float)DAT_005b8800 / (float)DAT_005b8804));
    }
  }
  else {
    iVar1 = FUN_0041cc70(0x5b6758);
    if (iVar1 == 7) {
      DAT_005b8818 = DAT_0056c8d0;
      DAT_005b8800 = 0xa00;
      DAT_005b8804 = 0x780;
    }
    else {
      iVar1 = FUN_0041cc70(0x5b6758);
      if (iVar1 == 6) {
        DAT_005b8818 = DAT_0056c8d0;
        DAT_005b8800 = 0x780;
        DAT_005b8804 = 0x5a0;
      }
      else {
        iVar1 = FUN_0041cc70(0x5b6758);
        if ((iVar1 == 2) || (iVar1 = FUN_0041cc70(0x5b6758), iVar1 == 5)) {
          DAT_005b8818 = DAT_0056c8d0;
          DAT_005b8800 = 0x500;
          DAT_005b8804 = 0x3c0;
        }
        else {
          iVar1 = FUN_0041cc70(0x5b6758);
          if ((iVar1 == 1) || (iVar1 = FUN_0041cc70(0x5b6758), iVar1 == 4)) {
            DAT_005b8818 = DAT_0056cd68;
            DAT_005b8800 = 0x3c0;
            DAT_005b8804 = 0x2d0;
          }
          else {
            DAT_005b8818 = DAT_0056c8cc;
            DAT_005b8800 = 0x280;
            DAT_005b8804 = 0x1e0;
          }
        }
      }
    }
  }
  DAT_005b87f8 = (int)(DAT_0056cdac * DAT_005b8818);
  DAT_005b87fc = (int)(DAT_0056cda8 * DAT_005b8818);
  FUN_00412da0(this,0x280);
  FUN_0041de50(this,0x1e0);
  FUN_0041de90(this,0x180);
  FUN_0041de70(this,0x1c0);
  iVar1 = FUN_00411700((int)this);
  FUN_0041df30(this,(DAT_005b87f8 - iVar1) / 2);
  iVar1 = FUN_0041ca90((int)this);
  FUN_0041df50(this,(DAT_005b87fc - iVar1) / 2);
  iVar1 = FUN_0041cad0((int)this);
  FUN_0041ded0(this,0,iVar1 + 0x20);
  iVar1 = FUN_0041cad0((int)this);
  FUN_0041ded0(this,1,iVar1 + 0x20);
  iVar1 = FUN_0041cad0((int)this);
  FUN_0041df10(this,0,iVar1 + 0x10);
  iVar1 = FUN_0041cad0((int)this);
  FUN_0041df10(this,1,iVar1 + 0x10);
  FUN_0041deb0(this,0,(int)(DAT_0056cd94 * DAT_005b8818));
  FUN_0041deb0(this,1,(int)(DAT_0056cd94 * DAT_005b8818));
  FUN_0041def0(this,0,(int)(DAT_0056cd90 * DAT_005b8818));
  FUN_0041def0(this,1,(int)(DAT_0056cd90 * DAT_005b8818));
  iVar1 = FUN_0041cad0((int)this);
  FUN_0041df70(this,0,iVar1 + (int)((DAT_0056cdac - DAT_0056cda4) / DAT_0056c8d0 +
                                   DAT_0056cda4 / DAT_0056c8d0));
  iVar1 = FUN_0041cad0((int)this);
  FUN_0041df70(this,1,iVar1 + (int)((DAT_0056cdac - DAT_0056cda4) / DAT_0056c8d0 +
                                   DAT_0056cda4 / DAT_0056c8d0));
  iVar1 = FUN_0041caf0((int)this);
  FUN_0041df90(this,0,iVar1 + 0x10);
  iVar1 = FUN_0041caf0((int)this);
  FUN_0041df90(this,1,iVar1 + 0x10);
  FUN_0041dfb0(this,DAT_005b87f8 / 2);
  FUN_0041dff0(this,(DAT_005b87fc + -0x1c0) / 2);
  FUN_0041dfd0(this,DAT_005b87f8 / 2);
  FUN_0041e010(this,(int)(DAT_0056cd90 * DAT_005b8818));
  return;
}

