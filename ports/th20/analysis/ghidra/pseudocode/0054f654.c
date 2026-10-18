/* Entry: 0x0054f654; symbol: FUN_0054f654; body bytes: 308 */

void __fastcall FUN_0054f654(int param_1)

{
  ushort uVar1;
  ushort *puVar2;
  ushort *puVar3;
  
  puVar2 = *(ushort **)(param_1 + 8);
  uVar1 = *puVar2;
  if (uVar1 < 0x6b) {
    if (uVar1 == 0x6a) {
      puVar3 = puVar2 + 1;
      *(undefined4 *)(param_1 + 0x28) = 5;
    }
    else if (uVar1 == 0x49) {
      uVar1 = puVar2[1];
      if (uVar1 == 0x33) {
        if (puVar2[2] != 0x32) {
          return;
        }
        puVar3 = puVar2 + 3;
      }
      else {
        if (uVar1 == 0x36) {
          if (puVar2[2] != 0x34) {
            return;
          }
          puVar3 = puVar2 + 3;
          *(undefined4 *)(param_1 + 0x28) = 10;
          goto LAB_0054f781;
        }
        if ((((uVar1 != 100) && (uVar1 != 0x69)) && (uVar1 != 0x6f)) &&
           (((uVar1 != 0x75 && (uVar1 != 0x78)) && (uVar1 != 0x58)))) {
          return;
        }
        puVar3 = puVar2 + 1;
      }
      *(undefined4 *)(param_1 + 0x28) = 9;
    }
    else if (uVar1 == 0x4c) {
      puVar3 = puVar2 + 1;
      *(undefined4 *)(param_1 + 0x28) = 8;
    }
    else {
      if (uVar1 != 0x54) {
        if (uVar1 != 0x68) {
          return;
        }
        puVar3 = puVar2 + 1;
        uVar1 = *puVar3;
        if (uVar1 == 0x68) {
          puVar3 = puVar2 + 2;
        }
        *(ushort **)(param_1 + 8) = puVar3;
        *(uint *)(param_1 + 0x28) = (uVar1 != 0x68) + 1;
        return;
      }
      puVar3 = puVar2 + 1;
      *(undefined4 *)(param_1 + 0x28) = 0xb;
    }
  }
  else if (uVar1 == 0x6c) {
    puVar3 = puVar2 + 1;
    if (*puVar3 == 0x6c) {
      puVar3 = puVar2 + 2;
      *(undefined4 *)(param_1 + 0x28) = 4;
    }
    else {
      *(undefined4 *)(param_1 + 0x28) = 3;
    }
  }
  else if (uVar1 == 0x74) {
    puVar3 = puVar2 + 1;
    *(undefined4 *)(param_1 + 0x28) = 7;
  }
  else {
    if (uVar1 != 0x7a) {
      return;
    }
    puVar3 = puVar2 + 1;
    *(undefined4 *)(param_1 + 0x28) = 6;
  }
LAB_0054f781:
  *(ushort **)(param_1 + 8) = puVar3;
  return;
}

