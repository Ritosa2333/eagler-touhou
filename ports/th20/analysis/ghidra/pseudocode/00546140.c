/* Entry: 0x00546140; symbol: FUN_00546140; body bytes: 299 */

uint * __cdecl FUN_00546140(uint *param_1,char param_2)

{
  int iVar1;
  char cVar2;
  uint uVar3;
  uint *puVar4;
  uint uVar5;
  uint uVar6;
  char *pcVar7;
  undefined1 auVar8 [16];
  undefined1 auVar9 [16];
  undefined1 auVar10 [16];
  
  if (DAT_005e5288 != 0) {
    auVar10 = pshuflw(ZEXT216(CONCAT11(param_2,param_2)),ZEXT216(CONCAT11(param_2,param_2)),0);
    uVar3 = -1 << (sbyte)((uint)param_1 & 0xf);
    pcVar7 = (char *)((int)param_1 - ((uint)param_1 & 0xf));
    while( true ) {
      auVar9[0] = -(*pcVar7 == '\0');
      auVar9[1] = -(pcVar7[1] == '\0');
      auVar9[2] = -(pcVar7[2] == '\0');
      auVar9[3] = -(pcVar7[3] == '\0');
      auVar9[4] = -(pcVar7[4] == '\0');
      auVar9[5] = -(pcVar7[5] == '\0');
      auVar9[6] = -(pcVar7[6] == '\0');
      auVar9[7] = -(pcVar7[7] == '\0');
      auVar9[8] = -(pcVar7[8] == '\0');
      auVar9[9] = -(pcVar7[9] == '\0');
      auVar9[10] = -(pcVar7[10] == '\0');
      auVar9[0xb] = -(pcVar7[0xb] == '\0');
      auVar9[0xc] = -(pcVar7[0xc] == '\0');
      auVar9[0xd] = -(pcVar7[0xd] == '\0');
      auVar9[0xe] = -(pcVar7[0xe] == '\0');
      auVar9[0xf] = -(pcVar7[0xf] == '\0');
      cVar2 = auVar10[0];
      auVar8[0] = -(*pcVar7 == cVar2);
      auVar8[1] = -(pcVar7[1] == auVar10[1]);
      auVar8[2] = -(pcVar7[2] == auVar10[2]);
      auVar8[3] = -(pcVar7[3] == auVar10[3]);
      auVar8[4] = -(pcVar7[4] == auVar10[4]);
      auVar8[5] = -(pcVar7[5] == auVar10[5]);
      auVar8[6] = -(pcVar7[6] == auVar10[6]);
      auVar8[7] = -(pcVar7[7] == auVar10[7]);
      auVar8[8] = -(pcVar7[8] == cVar2);
      auVar8[9] = -(pcVar7[9] == auVar10[1]);
      auVar8[10] = -(pcVar7[10] == auVar10[2]);
      auVar8[0xb] = -(pcVar7[0xb] == auVar10[3]);
      auVar8[0xc] = -(pcVar7[0xc] == auVar10[4]);
      auVar8[0xd] = -(pcVar7[0xd] == auVar10[5]);
      auVar8[0xe] = -(pcVar7[0xe] == auVar10[6]);
      auVar8[0xf] = -(pcVar7[0xf] == auVar10[7]);
      auVar9 = auVar9 | auVar8;
      uVar3 = (ushort)((ushort)(SUB161(auVar9 >> 7,0) & 1) |
                       (ushort)(SUB161(auVar9 >> 0xf,0) & 1) << 1 |
                       (ushort)(SUB161(auVar9 >> 0x17,0) & 1) << 2 |
                       (ushort)(SUB161(auVar9 >> 0x1f,0) & 1) << 3 |
                       (ushort)(SUB161(auVar9 >> 0x27,0) & 1) << 4 |
                       (ushort)(SUB161(auVar9 >> 0x2f,0) & 1) << 5 |
                       (ushort)(SUB161(auVar9 >> 0x37,0) & 1) << 6 |
                       (ushort)(SUB161(auVar9 >> 0x3f,0) & 1) << 7 |
                       (ushort)(SUB161(auVar9 >> 0x47,0) & 1) << 8 |
                       (ushort)(SUB161(auVar9 >> 0x4f,0) & 1) << 9 |
                       (ushort)(SUB161(auVar9 >> 0x57,0) & 1) << 10 |
                       (ushort)(SUB161(auVar9 >> 0x5f,0) & 1) << 0xb |
                       (ushort)(SUB161(auVar9 >> 0x67,0) & 1) << 0xc |
                       (ushort)(SUB161(auVar9 >> 0x6f,0) & 1) << 0xd |
                       (ushort)(SUB161(auVar9 >> 0x77,0) & 1) << 0xe |
                      (ushort)(byte)(auVar9[0xf] >> 7) << 0xf) & uVar3;
      if (uVar3 != 0) break;
      uVar3 = 0xffffffff;
      pcVar7 = pcVar7 + 0x10;
    }
    iVar1 = 0;
    if (uVar3 != 0) {
      for (; (uVar3 >> iVar1 & 1) == 0; iVar1 = iVar1 + 1) {
      }
    }
    puVar4 = (uint *)(pcVar7 + iVar1);
    if (cVar2 != (char)*puVar4) {
      puVar4 = (uint *)0x0;
    }
    return puVar4;
  }
  uVar3 = (uint)param_1 & 3;
  while (uVar3 != 0) {
    if ((char)*param_1 == param_2) {
      return param_1;
    }
    if ((char)*param_1 == '\0') {
      return (uint *)0x0;
    }
    uVar3 = (uint)((int)param_1 + 1) & 3;
    param_1 = (uint *)((int)param_1 + 1);
  }
  while( true ) {
    while( true ) {
      uVar3 = *param_1;
      uVar6 = uVar3 ^ CONCAT22(CONCAT11(param_2,param_2),CONCAT11(param_2,param_2));
      uVar5 = uVar3 ^ 0xffffffff ^ uVar3 + 0x7efefeff;
      puVar4 = param_1 + 1;
      if (((uVar6 ^ 0xffffffff ^ uVar6 + 0x7efefeff) & 0x81010100) != 0) break;
      param_1 = puVar4;
      if ((uVar5 & 0x81010100) != 0) {
        if ((uVar5 & 0x1010100) != 0) {
          return (uint *)0x0;
        }
        if ((uVar3 + 0x7efefeff & 0x80000000) == 0) {
          return (uint *)0x0;
        }
      }
    }
    uVar3 = *param_1;
    if ((char)uVar3 == param_2) {
      return param_1;
    }
    if ((char)uVar3 == '\0') {
      return (uint *)0x0;
    }
    cVar2 = (char)(uVar3 >> 8);
    if (cVar2 == param_2) {
      return (uint *)((int)param_1 + 1);
    }
    if (cVar2 == '\0') break;
    cVar2 = (char)(uVar3 >> 0x10);
    if (cVar2 == param_2) {
      return (uint *)((int)param_1 + 2);
    }
    if (cVar2 == '\0') {
      return (uint *)0x0;
    }
    cVar2 = (char)(uVar3 >> 0x18);
    if (cVar2 == param_2) {
      return (uint *)((int)param_1 + 3);
    }
    param_1 = puVar4;
    if (cVar2 == '\0') {
      return (uint *)0x0;
    }
  }
  return (uint *)0x0;
}

