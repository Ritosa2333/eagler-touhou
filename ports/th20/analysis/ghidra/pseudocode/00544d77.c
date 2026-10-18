/* Entry: 0x00544d77; symbol: FUN_00544d77; body bytes: 4929 */

int __cdecl FUN_00544d77(uint *param_1,uint *param_2,uint param_3)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  
  if (param_3 == 0) {
    return 0;
  }
  if (param_3 == 1) {
    if ((uint)(byte)*param_1 == (uint)(byte)*param_2) {
      return 0;
    }
    if ((int)((uint)(byte)*param_1 - (uint)(byte)*param_2) < 1) {
      return -1;
    }
    return 1;
  }
  if (param_3 == 2) {
    iVar5 = (uint)(byte)*param_1 - (uint)(byte)*param_2;
    iVar6 = iVar5;
    if ((iVar5 != 0) && (iVar6 = -1, 0 < iVar5)) {
      iVar6 = 1;
    }
    if (iVar6 != 0) {
      return iVar6;
    }
    uVar4 = (uint)*(byte *)((int)param_1 + 1);
    uVar1 = (uint)*(byte *)((int)param_2 + 1);
  }
  else if (param_3 == 3) {
    iVar5 = (uint)(byte)*param_1 - (uint)(byte)*param_2;
    iVar6 = iVar5;
    if ((iVar5 != 0) && (iVar6 = -1, 0 < iVar5)) {
      iVar6 = 1;
    }
    if (iVar6 != 0) {
      return iVar6;
    }
    iVar5 = (uint)*(byte *)((int)param_1 + 1) - (uint)*(byte *)((int)param_2 + 1);
    iVar6 = iVar5;
    if ((iVar5 != 0) && (iVar6 = -1, 0 < iVar5)) {
      iVar6 = 1;
    }
    if (iVar6 != 0) {
      return iVar6;
    }
    uVar4 = (uint)*(byte *)((int)param_1 + 2);
    uVar1 = (uint)*(byte *)((int)param_2 + 2);
  }
  else if (param_3 == 4) {
    iVar6 = -1;
    iVar5 = 1;
    iVar3 = (uint)(byte)*param_1 - (uint)(byte)*param_2;
    iVar2 = iVar3;
    if ((iVar3 != 0) && (iVar2 = iVar6, 0 < iVar3)) {
      iVar2 = iVar5;
    }
    if (iVar2 != 0) {
      return iVar2;
    }
    iVar3 = (uint)*(byte *)((int)param_1 + 1) - (uint)*(byte *)((int)param_2 + 1);
    iVar2 = iVar3;
    if ((iVar3 != 0) && (iVar2 = iVar6, 0 < iVar3)) {
      iVar2 = iVar5;
    }
    if (iVar2 != 0) {
      return iVar2;
    }
    iVar3 = (uint)*(byte *)((int)param_1 + 2) - (uint)*(byte *)((int)param_2 + 2);
    iVar2 = iVar3;
    if ((iVar3 != 0) && (iVar2 = iVar6, 0 < iVar3)) {
      iVar2 = iVar5;
    }
    if (iVar2 != 0) {
      return iVar2;
    }
    uVar4 = (uint)*(byte *)((int)param_1 + 3);
    uVar1 = (uint)*(byte *)((int)param_2 + 3);
  }
  else {
    iVar6 = 1;
    if (0x1f < param_3) {
      do {
        if (*param_1 != *param_2) {
          iVar2 = (*param_1 & 0xff) - (uint)(byte)*param_2;
          iVar5 = iVar2;
          if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
            iVar5 = iVar6;
          }
          if (iVar5 != 0) {
            return iVar5;
          }
          iVar2 = (uint)*(byte *)((int)param_1 + 1) - (uint)*(byte *)((int)param_2 + 1);
          iVar5 = iVar2;
          if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
            iVar5 = iVar6;
          }
          if (iVar5 != 0) {
            return iVar5;
          }
          iVar2 = (uint)*(byte *)((int)param_1 + 2) - (uint)*(byte *)((int)param_2 + 2);
          iVar5 = iVar2;
          if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
            iVar5 = iVar6;
          }
          if (iVar5 != 0) {
            return iVar5;
          }
          iVar2 = (uint)*(byte *)((int)param_1 + 3) - (uint)*(byte *)((int)param_2 + 3);
          iVar5 = iVar2;
          if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
            iVar5 = iVar6;
          }
          if (iVar5 != 0) {
            return iVar5;
          }
        }
        if (param_1[1] != param_2[1]) {
          iVar2 = (uint)(byte)param_1[1] - (uint)(byte)param_2[1];
          iVar5 = iVar2;
          if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
            iVar5 = iVar6;
          }
          if (iVar5 != 0) {
            return iVar5;
          }
          iVar2 = (uint)*(byte *)((int)param_1 + 5) - (uint)*(byte *)((int)param_2 + 5);
          iVar5 = iVar2;
          if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
            iVar5 = iVar6;
          }
          if (iVar5 != 0) {
            return iVar5;
          }
          iVar2 = (uint)*(byte *)((int)param_1 + 6) - (uint)*(byte *)((int)param_2 + 6);
          iVar5 = iVar2;
          if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
            iVar5 = iVar6;
          }
          if (iVar5 != 0) {
            return iVar5;
          }
          iVar2 = (uint)*(byte *)((int)param_1 + 7) - (uint)*(byte *)((int)param_2 + 7);
          iVar5 = iVar2;
          if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
            iVar5 = iVar6;
          }
          if (iVar5 != 0) {
            return iVar5;
          }
        }
        if (param_1[2] != param_2[2]) {
          iVar2 = (param_1[2] & 0xff) - (uint)(byte)param_2[2];
          iVar5 = iVar2;
          if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
            iVar5 = iVar6;
          }
          if (iVar5 != 0) {
            return iVar5;
          }
          iVar2 = (uint)*(byte *)((int)param_1 + 9) - (uint)*(byte *)((int)param_2 + 9);
          iVar5 = iVar2;
          if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
            iVar5 = iVar6;
          }
          if (iVar5 != 0) {
            return iVar5;
          }
          iVar2 = (uint)*(byte *)((int)param_1 + 10) - (uint)*(byte *)((int)param_2 + 10);
          iVar5 = iVar2;
          if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
            iVar5 = iVar6;
          }
          if (iVar5 != 0) {
            return iVar5;
          }
          iVar2 = (uint)*(byte *)((int)param_1 + 0xb) - (uint)*(byte *)((int)param_2 + 0xb);
          iVar5 = iVar2;
          if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
            iVar5 = iVar6;
          }
          if (iVar5 != 0) {
            return iVar5;
          }
        }
        if (param_1[3] != param_2[3]) {
          iVar2 = (uint)(byte)param_1[3] - (uint)(byte)param_2[3];
          iVar5 = iVar2;
          if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
            iVar5 = iVar6;
          }
          if (iVar5 != 0) {
            return iVar5;
          }
          iVar2 = (uint)*(byte *)((int)param_1 + 0xd) - (uint)*(byte *)((int)param_2 + 0xd);
          iVar5 = iVar2;
          if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
            iVar5 = iVar6;
          }
          if (iVar5 != 0) {
            return iVar5;
          }
          iVar2 = (uint)*(byte *)((int)param_1 + 0xe) - (uint)*(byte *)((int)param_2 + 0xe);
          iVar5 = iVar2;
          if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
            iVar5 = iVar6;
          }
          if (iVar5 != 0) {
            return iVar5;
          }
          iVar2 = (uint)*(byte *)((int)param_1 + 0xf) - (uint)*(byte *)((int)param_2 + 0xf);
          iVar5 = iVar2;
          if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
            iVar5 = iVar6;
          }
          if (iVar5 != 0) {
            return iVar5;
          }
        }
        if (param_1[4] != param_2[4]) {
          iVar2 = (param_1[4] & 0xff) - (uint)(byte)param_2[4];
          iVar5 = iVar2;
          if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
            iVar5 = iVar6;
          }
          if (iVar5 != 0) {
            return iVar5;
          }
          iVar2 = (uint)*(byte *)((int)param_1 + 0x11) - (uint)*(byte *)((int)param_2 + 0x11);
          iVar5 = iVar2;
          if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
            iVar5 = iVar6;
          }
          if (iVar5 != 0) {
            return iVar5;
          }
          iVar2 = (uint)*(byte *)((int)param_1 + 0x12) - (uint)*(byte *)((int)param_2 + 0x12);
          iVar5 = iVar2;
          if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
            iVar5 = iVar6;
          }
          if (iVar5 != 0) {
            return iVar5;
          }
          iVar2 = (uint)*(byte *)((int)param_1 + 0x13) - (uint)*(byte *)((int)param_2 + 0x13);
          iVar5 = iVar2;
          if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
            iVar5 = iVar6;
          }
          if (iVar5 != 0) {
            return iVar5;
          }
        }
        if (param_1[5] != param_2[5]) {
          iVar2 = (uint)(byte)param_1[5] - (uint)(byte)param_2[5];
          iVar5 = iVar2;
          if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
            iVar5 = iVar6;
          }
          if (iVar5 != 0) {
            return iVar5;
          }
          iVar2 = (uint)*(byte *)((int)param_1 + 0x15) - (uint)*(byte *)((int)param_2 + 0x15);
          iVar5 = iVar2;
          if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
            iVar5 = iVar6;
          }
          if (iVar5 != 0) {
            return iVar5;
          }
          iVar2 = (uint)*(byte *)((int)param_1 + 0x16) - (uint)*(byte *)((int)param_2 + 0x16);
          iVar5 = iVar2;
          if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
            iVar5 = iVar6;
          }
          if (iVar5 != 0) {
            return iVar5;
          }
          iVar2 = (uint)*(byte *)((int)param_1 + 0x17) - (uint)*(byte *)((int)param_2 + 0x17);
          iVar5 = iVar2;
          if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
            iVar5 = iVar6;
          }
          if (iVar5 != 0) {
            return iVar5;
          }
        }
        if (param_1[6] != param_2[6]) {
          iVar2 = (param_1[6] & 0xff) - (uint)(byte)param_2[6];
          iVar5 = iVar2;
          if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
            iVar5 = iVar6;
          }
          if (iVar5 != 0) {
            return iVar5;
          }
          iVar2 = (uint)*(byte *)((int)param_1 + 0x19) - (uint)*(byte *)((int)param_2 + 0x19);
          iVar5 = iVar2;
          if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
            iVar5 = iVar6;
          }
          if (iVar5 != 0) {
            return iVar5;
          }
          iVar2 = (uint)*(byte *)((int)param_1 + 0x1a) - (uint)*(byte *)((int)param_2 + 0x1a);
          iVar5 = iVar2;
          if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
            iVar5 = iVar6;
          }
          if (iVar5 != 0) {
            return iVar5;
          }
          iVar2 = (uint)*(byte *)((int)param_1 + 0x1b) - (uint)*(byte *)((int)param_2 + 0x1b);
          iVar5 = iVar2;
          if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
            iVar5 = iVar6;
          }
          if (iVar5 != 0) {
            return iVar5;
          }
        }
        if (param_1[7] != param_2[7]) {
          iVar2 = (uint)(byte)param_1[7] - (uint)(byte)param_2[7];
          iVar5 = iVar2;
          if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
            iVar5 = iVar6;
          }
          if (iVar5 != 0) {
            return iVar5;
          }
          iVar2 = (uint)*(byte *)((int)param_1 + 0x1d) - (uint)*(byte *)((int)param_2 + 0x1d);
          iVar5 = iVar2;
          if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
            iVar5 = iVar6;
          }
          if (iVar5 != 0) {
            return iVar5;
          }
          iVar2 = (uint)*(byte *)((int)param_1 + 0x1e) - (uint)*(byte *)((int)param_2 + 0x1e);
          iVar5 = iVar2;
          if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
            iVar5 = iVar6;
          }
          if (iVar5 != 0) {
            return iVar5;
          }
          iVar2 = (uint)*(byte *)((int)param_1 + 0x1f) - (uint)*(byte *)((int)param_2 + 0x1f);
          iVar5 = iVar2;
          if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
            iVar5 = iVar6;
          }
          if (iVar5 != 0) {
            return iVar5;
          }
        }
        param_3 = param_3 - 0x20;
        param_1 = param_1 + 8;
        param_2 = param_2 + 8;
      } while (0x1f < param_3);
    }
    switch(param_3) {
    case 0:
      return 0;
    case 0x1c:
      if (*(int *)((int)param_1 + (param_3 - 0x1c)) != *(int *)((int)param_2 + (param_3 - 0x1c))) {
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0x1c)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0x1c));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0x1b)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0x1b));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0x1a)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0x1a));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0x19)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0x19));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
      }
    case 0x18:
      if (*(int *)((int)param_1 + (param_3 - 0x18)) != *(int *)((int)param_2 + (param_3 - 0x18))) {
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0x18)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0x18));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0x17)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0x17));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0x16)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0x16));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0x15)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0x15));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
      }
    case 0x14:
      if (*(int *)((int)param_1 + (param_3 - 0x14)) != *(int *)((int)param_2 + (param_3 - 0x14))) {
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0x14)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0x14));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0x13)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0x13));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0x12)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0x12));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0x11)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0x11));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
      }
    case 0x10:
      if (*(int *)((int)param_1 + (param_3 - 0x10)) != *(int *)((int)param_2 + (param_3 - 0x10))) {
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0x10)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0x10));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0xf)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0xf));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0xe)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0xe));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0xd)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0xd));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
      }
    case 0xc:
      if (*(int *)((int)param_1 + (param_3 - 0xc)) != *(int *)((int)param_2 + (param_3 - 0xc))) {
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0xc)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0xc));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0xb)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0xb));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 10)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 10));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 9)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 9));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
      }
    case 8:
      if (*(int *)((int)param_1 + (param_3 - 8)) != *(int *)((int)param_2 + (param_3 - 8))) {
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 8)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 8));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 7)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 7));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 6)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 6));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 5)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 5));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
      }
    case 4:
      uVar1 = *(uint *)((int)param_1 + (param_3 - 4));
      if (uVar1 == *(uint *)((int)param_2 + (param_3 - 4))) {
        return 0;
      }
      iVar2 = (uVar1 & 0xff) - (uint)*(byte *)((int)param_2 + (param_3 - 4));
      iVar5 = iVar2;
      if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
        iVar5 = iVar6;
      }
      if (iVar5 != 0) {
        return iVar5;
      }
      iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 3)) -
              (uint)*(byte *)((int)param_2 + (param_3 - 3));
      iVar5 = iVar2;
      if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
        iVar5 = iVar6;
      }
      if (iVar5 != 0) {
        return iVar5;
      }
      iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 2)) -
              (uint)*(byte *)((int)param_2 + (param_3 - 2));
      iVar5 = iVar2;
      if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
        iVar5 = iVar6;
      }
      if (iVar5 == 0) {
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 1)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 1));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 == 0) {
          return 0;
        }
        return iVar5;
      }
      return iVar5;
    case 0x1d:
      uVar1 = *(uint *)((int)param_1 + (param_3 - 0x1d));
      if (uVar1 != *(uint *)((int)param_2 + (param_3 - 0x1d))) {
        iVar2 = (uVar1 & 0xff) - (uint)*(byte *)((int)param_2 + (param_3 - 0x1d));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0x1c)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0x1c));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0x1b)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0x1b));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0x1a)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0x1a));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
      }
    case 0x19:
      if (*(int *)((int)param_1 + (param_3 - 0x19)) != *(int *)((int)param_2 + (param_3 - 0x19))) {
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0x19)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0x19));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0x18)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0x18));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0x17)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0x17));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0x16)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0x16));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
      }
    case 0x15:
      if (*(int *)((int)param_1 + (param_3 - 0x15)) != *(int *)((int)param_2 + (param_3 - 0x15))) {
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0x15)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0x15));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0x14)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0x14));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0x13)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0x13));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0x12)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0x12));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
      }
    case 0x11:
      uVar1 = *(uint *)((int)param_1 + (param_3 - 0x11));
      if (uVar1 != *(uint *)((int)param_2 + (param_3 - 0x11))) {
        iVar2 = (uVar1 & 0xff) - (uint)*(byte *)((int)param_2 + (param_3 - 0x11));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0x10)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0x10));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0xf)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0xf));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0xe)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0xe));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
      }
    case 0xd:
      if (*(int *)((int)param_1 + (param_3 - 0xd)) != *(int *)((int)param_2 + (param_3 - 0xd))) {
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0xd)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0xd));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0xc)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0xc));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0xb)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0xb));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 10)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 10));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
      }
    case 9:
      uVar1 = *(uint *)((int)param_1 + (param_3 - 9));
      if (uVar1 != *(uint *)((int)param_2 + (param_3 - 9))) {
        iVar2 = (uVar1 & 0xff) - (uint)*(byte *)((int)param_2 + (param_3 - 9));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 8)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 8));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 7)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 7));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 6)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 6));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
      }
    case 5:
      if (*(int *)((int)param_1 + (param_3 - 5)) != *(int *)((int)param_2 + (param_3 - 5))) {
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 5)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 5));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 4)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 4));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 3)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 3));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 2)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 2));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
      }
    case 1:
      uVar4 = (uint)*(byte *)((int)param_1 + (param_3 - 1));
      uVar1 = (uint)*(byte *)((int)param_2 + (param_3 - 1));
      goto LAB_00546003;
    case 0x1e:
      uVar1 = *(uint *)((int)param_1 + (param_3 - 0x1e));
      if (uVar1 != *(uint *)((int)param_2 + (param_3 - 0x1e))) {
        iVar2 = (uVar1 & 0xff) - (uint)*(byte *)((int)param_2 + (param_3 - 0x1e));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0x1d)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0x1d));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0x1c)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0x1c));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0x1b)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0x1b));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
      }
    case 0x1a:
      uVar1 = *(uint *)((int)param_1 + (param_3 - 0x1a));
      if (uVar1 != *(uint *)((int)param_2 + (param_3 - 0x1a))) {
        iVar2 = (uVar1 & 0xff) - (uint)*(byte *)((int)param_2 + (param_3 - 0x1a));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0x19)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0x19));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0x18)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0x18));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0x17)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0x17));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
      }
    case 0x16:
      uVar1 = *(uint *)((int)param_1 + (param_3 - 0x16));
      if (uVar1 != *(uint *)((int)param_2 + (param_3 - 0x16))) {
        iVar2 = (uVar1 & 0xff) - (uint)*(byte *)((int)param_2 + (param_3 - 0x16));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0x15)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0x15));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0x14)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0x14));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0x13)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0x13));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
      }
    case 0x12:
      uVar1 = *(uint *)((int)param_1 + (param_3 - 0x12));
      if (uVar1 != *(uint *)((int)param_2 + (param_3 - 0x12))) {
        iVar2 = (uVar1 & 0xff) - (uint)*(byte *)((int)param_2 + (param_3 - 0x12));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0x11)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0x11));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0x10)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0x10));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0xf)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0xf));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
      }
    case 0xe:
      uVar1 = *(uint *)((int)param_1 + (param_3 - 0xe));
      if (uVar1 != *(uint *)((int)param_2 + (param_3 - 0xe))) {
        iVar2 = (uVar1 & 0xff) - (uint)*(byte *)((int)param_2 + (param_3 - 0xe));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0xd)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0xd));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0xc)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0xc));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0xb)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0xb));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
      }
    case 10:
      if (*(int *)((int)param_1 + (param_3 - 10)) != *(int *)((int)param_2 + (param_3 - 10))) {
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 10)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 10));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 9)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 9));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 8)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 8));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 7)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 7));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
      }
    case 6:
      uVar1 = *(uint *)((int)param_1 + (param_3 - 6));
      if (uVar1 != *(uint *)((int)param_2 + (param_3 - 6))) {
        iVar2 = (uVar1 & 0xff) - (uint)*(byte *)((int)param_2 + (param_3 - 6));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 5)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 5));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 4)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 4));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 3)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 3));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
      }
    case 2:
      if (*(short *)((int)param_1 + (param_3 - 2)) == *(short *)((int)param_2 + (param_3 - 2))) {
        return 0;
      }
      break;
    case 0x1f:
      if (*(int *)((int)param_1 + (param_3 - 0x1f)) != *(int *)((int)param_2 + (param_3 - 0x1f))) {
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0x1f)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0x1f));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0x1e)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0x1e));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0x1d)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0x1d));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0x1c)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0x1c));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
      }
    case 0x1b:
      uVar1 = *(uint *)((int)param_1 + (param_3 - 0x1b));
      if (uVar1 != *(uint *)((int)param_2 + (param_3 - 0x1b))) {
        iVar2 = (uVar1 & 0xff) - (uint)*(byte *)((int)param_2 + (param_3 - 0x1b));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0x1a)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0x1a));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0x19)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0x19));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0x18)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0x18));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
      }
    case 0x17:
      uVar1 = *(uint *)((int)param_1 + (param_3 - 0x17));
      if (uVar1 != *(uint *)((int)param_2 + (param_3 - 0x17))) {
        iVar2 = (uVar1 & 0xff) - (uint)*(byte *)((int)param_2 + (param_3 - 0x17));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0x16)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0x16));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0x15)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0x15));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0x14)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0x14));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
      }
    case 0x13:
      if (*(int *)((int)param_1 + (param_3 - 0x13)) != *(int *)((int)param_2 + (param_3 - 0x13))) {
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0x13)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0x13));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0x12)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0x12));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0x11)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0x11));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0x10)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0x10));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
      }
    case 0xf:
      uVar1 = *(uint *)((int)param_1 + (param_3 - 0xf));
      if (uVar1 != *(uint *)((int)param_2 + (param_3 - 0xf))) {
        iVar2 = (uVar1 & 0xff) - (uint)*(byte *)((int)param_2 + (param_3 - 0xf));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0xe)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0xe));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0xd)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0xd));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 0xc)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 0xc));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
      }
    case 0xb:
      uVar1 = *(uint *)((int)param_1 + (param_3 - 0xb));
      if (uVar1 != *(uint *)((int)param_2 + (param_3 - 0xb))) {
        iVar2 = (uVar1 & 0xff) - (uint)*(byte *)((int)param_2 + (param_3 - 0xb));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 10)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 10));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 9)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 9));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 8)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 8));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
      }
    case 7:
      if (*(int *)((int)param_1 + (param_3 - 7)) != *(int *)((int)param_2 + (param_3 - 7))) {
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 7)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 7));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 6)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 6));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 5)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 5));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
        iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 4)) -
                (uint)*(byte *)((int)param_2 + (param_3 - 4));
        iVar5 = iVar2;
        if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
          iVar5 = iVar6;
        }
        if (iVar5 != 0) {
          return iVar5;
        }
      }
    case 3:
      iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 3)) -
              (uint)*(byte *)((int)param_2 + (param_3 - 3));
      iVar5 = iVar2;
      if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
        iVar5 = iVar6;
      }
      if (iVar5 != 0) {
        return iVar5;
      }
    }
    iVar2 = (uint)*(byte *)((int)param_1 + (param_3 - 2)) -
            (uint)*(byte *)((int)param_2 + (param_3 - 2));
    iVar5 = iVar2;
    if ((iVar2 != 0) && (iVar5 = -1, 0 < iVar2)) {
      iVar5 = iVar6;
    }
    if (iVar5 != 0) {
      return iVar5;
    }
    uVar1 = (uint)*(byte *)((int)param_2 + (param_3 - 1));
    uVar4 = (uint)*(byte *)((int)param_1 + (param_3 - 1));
  }
LAB_00546003:
  if (uVar4 == uVar1) {
    return 0;
  }
  if ((int)(uVar4 - uVar1) < 1) {
    return -1;
  }
  return 1;
}

