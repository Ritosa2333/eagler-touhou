/* Entry: 0x00552f7c; symbol: FUN_00552f7c; body bytes: 370 */

void __cdecl FUN_00552f7c(byte *param_1,undefined4 *param_2,byte *param_3,int *param_4,int *param_5)

{
  bool bVar1;
  byte bVar2;
  uint uVar3;
  int iVar4;
  bool bVar5;
  byte *pbVar6;
  
  *param_5 = 0;
  *param_4 = 1;
  if (param_2 != (undefined4 *)0x0) {
    *param_2 = param_3;
    param_2 = param_2 + 1;
  }
  bVar5 = false;
  do {
    if (*param_1 == 0x22) {
      bVar5 = !bVar5;
      bVar2 = 0x22;
      pbVar6 = param_1 + 1;
    }
    else {
      *param_5 = *param_5 + 1;
      if (param_3 != (byte *)0x0) {
        *param_3 = *param_1;
        param_3 = param_3 + 1;
      }
      bVar2 = *param_1;
      pbVar6 = param_1 + 1;
      iVar4 = FUN_0055f086(bVar2);
      if (iVar4 != 0) {
        *param_5 = *param_5 + 1;
        if (param_3 != (byte *)0x0) {
          *param_3 = *pbVar6;
          param_3 = param_3 + 1;
        }
        pbVar6 = param_1 + 2;
      }
      if (bVar2 == 0) {
        pbVar6 = pbVar6 + -1;
        goto LAB_0055300c;
      }
    }
    param_1 = pbVar6;
  } while ((bVar5) || ((bVar2 != 0x20 && (bVar2 != 9))));
  if (param_3 != (byte *)0x0) {
    param_3[-1] = 0;
  }
LAB_0055300c:
  bVar5 = false;
  while (bVar2 = *pbVar6, bVar2 != 0) {
    while ((bVar2 == 0x20 || (bVar2 == 9))) {
      pbVar6 = pbVar6 + 1;
      bVar2 = *pbVar6;
    }
    if (bVar2 == 0) break;
    if (param_2 != (undefined4 *)0x0) {
      *param_2 = param_3;
      param_2 = param_2 + 1;
    }
    *param_4 = *param_4 + 1;
    while( true ) {
      bVar1 = true;
      uVar3 = 0;
      for (; *pbVar6 == 0x5c; pbVar6 = pbVar6 + 1) {
        uVar3 = uVar3 + 1;
      }
      if (*pbVar6 == 0x22) {
        if ((uVar3 & 1) == 0) {
          if ((bVar5) && (pbVar6[1] == 0x22)) {
            pbVar6 = pbVar6 + 1;
          }
          else {
            bVar1 = false;
            bVar5 = !bVar5;
          }
        }
        uVar3 = uVar3 >> 1;
      }
      while (uVar3 != 0) {
        uVar3 = uVar3 - 1;
        if (param_3 != (byte *)0x0) {
          *param_3 = 0x5c;
          param_3 = param_3 + 1;
        }
        *param_5 = *param_5 + 1;
      }
      bVar2 = *pbVar6;
      if ((bVar2 == 0) || ((!bVar5 && ((bVar2 == 0x20 || (bVar2 == 9)))))) break;
      if (bVar1) {
        if (param_3 != (byte *)0x0) {
          *param_3 = bVar2;
          param_3 = param_3 + 1;
        }
        iVar4 = FUN_0055f086(*pbVar6);
        if (iVar4 != 0) {
          pbVar6 = pbVar6 + 1;
          *param_5 = *param_5 + 1;
          if (param_3 != (byte *)0x0) {
            *param_3 = *pbVar6;
            param_3 = param_3 + 1;
          }
        }
        *param_5 = *param_5 + 1;
      }
      pbVar6 = pbVar6 + 1;
    }
    if (param_3 != (byte *)0x0) {
      *param_3 = 0;
      param_3 = param_3 + 1;
    }
    *param_5 = *param_5 + 1;
  }
  if (param_2 != (undefined4 *)0x0) {
    *param_2 = 0;
  }
  *param_4 = *param_4 + 1;
  return;
}

