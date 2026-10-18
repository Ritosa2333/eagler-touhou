/* Entry: 0x00541f37; symbol: FUN_00541f37; body bytes: 255 */

void FUN_00541f37(UINT param_1,UINT *param_2)

{
  BOOL BVar1;
  int iVar2;
  uint uVar3;
  char cVar4;
  int iVar5;
  uint uVar6;
  UINT *pUVar7;
  UINT local_260 [11];
  UINT *local_234;
  int local_230;
  BYTE local_22a;
  BYTE local_229;
  _cpinfoexW local_228;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  iVar5 = 0;
  local_234 = param_2;
  local_230 = 0;
  _memset(local_260,0,0x2c);
  pUVar7 = local_260;
  for (iVar2 = 0xb; iVar2 != 0; iVar2 = iVar2 + -1) {
    *param_2 = *pUVar7;
    pUVar7 = pUVar7 + 1;
    param_2 = param_2 + 1;
  }
  _memset(&local_228,0,0x220);
  BVar1 = GetCPInfoExW(param_1,0,&local_228);
  if (BVar1 == 0) {
    GetLastError();
  }
  else {
    *local_234 = local_228.CodePage;
    local_234[1] = local_228.MaxCharSize;
    do {
      local_229 = local_228.LeadByte[iVar5];
      if ((local_229 == '\0') && (local_228.LeadByte[iVar5 + 1] == '\0')) break;
      local_22a = local_228.LeadByte[iVar5 + 1];
      if (local_229 != local_22a) {
        uVar6 = (uint)CONCAT11(local_22a,local_229);
        do {
          uVar3 = (uVar6 & 0xff) >> 3;
          cVar4 = (char)uVar6 + '\x01';
          *(byte *)(uVar3 + 0xc + (int)local_234) =
               *(byte *)(uVar3 + 0xc + (int)local_234) | (byte)(1 << (uVar6 & 7));
          uVar3 = uVar6 >> 8;
          iVar5 = local_230;
          uVar6 = CONCAT31((int3)(uVar6 >> 8),cVar4);
        } while (cVar4 != (char)uVar3);
      }
      iVar5 = iVar5 + 2;
      local_230 = iVar5;
    } while (iVar5 < 0xc);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

