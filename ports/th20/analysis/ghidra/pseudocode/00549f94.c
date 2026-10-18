/* Entry: 0x00549f94; symbol: FUN_00549f94; body bytes: 439 */

undefined4 __fastcall FUN_00549f94(uint *param_1)

{
  char *pcVar1;
  char cVar2;
  undefined4 *puVar3;
  int *piVar4;
  uint *puVar5;
  bool bVar6;
  formatting_buffer *pfVar7;
  char *pcVar8;
  uint uVar9;
  formatting_buffer *pfVar10;
  uint *puVar11;
  uint uVar12;
  undefined4 local_20;
  undefined4 local_1c;
  uint *local_18;
  char *local_14;
  uint *local_10;
  uint local_c;
  formatting_buffer *local_8;
  
  param_1[7] = param_1[7] | 0x10;
  uVar12 = param_1[9];
  if ((int)uVar12 < 0) {
    if ((*(char *)((int)param_1 + 0x2d) == 'a') || (*(char *)((int)param_1 + 0x2d) == 'A')) {
      uVar12 = 0xd;
    }
    else {
      uVar12 = 6;
    }
    param_1[9] = uVar12;
  }
  else if ((uVar12 == 0) &&
          ((*(char *)((int)param_1 + 0x2d) == 'g' || (*(char *)((int)param_1 + 0x2d) == 'G')))) {
    param_1[9] = 1;
    uVar12 = 1;
  }
  pfVar10 = (formatting_buffer *)(param_1 + 0xf);
  local_10 = param_1;
  local_8 = pfVar10;
  bVar6 = FUN_00548966(pfVar10,uVar12 + 0x15d,param_1[2]);
  if (bVar6) {
    local_c = param_1[9];
  }
  else {
    local_c = __crt_stdio_output::formatting_buffer::count<char>(pfVar10);
    local_c = local_c - 0x15d;
    param_1[9] = local_c;
  }
  local_18 = param_1 + 0xc;
  pfVar7 = (formatting_buffer *)param_1[0x110];
  if ((formatting_buffer *)param_1[0x110] == (formatting_buffer *)0x0) {
    pfVar7 = pfVar10;
  }
  *local_18 = (uint)pfVar7;
  puVar3 = (undefined4 *)param_1[4];
  param_1[4] = (uint)(puVar3 + 2);
  local_20 = *puVar3;
  local_1c = puVar3[1];
  local_14 = (char *)((int)param_1 + 0x2d);
  cVar2 = *local_14;
  uVar12 = __crt_stdio_output::formatting_buffer::count<char>(pfVar10);
  pcVar8 = __crt_stdio_output::formatting_buffer::scratch_data<char>(pfVar10);
  uVar9 = __crt_stdio_output::formatting_buffer::count<char>(local_8);
  pfVar10 = *(formatting_buffer **)(local_8 + 0x404);
  if (*(formatting_buffer **)(local_8 + 0x404) == (formatting_buffer *)0x0) {
    pfVar10 = local_8;
  }
  FUN_00558f7c((double *)&local_20,(undefined1 (*) [16])pfVar10,uVar9,pcVar8,uVar12,(int)cVar2,
               local_c,*local_10,local_10[1],1,(int *)local_10[2]);
  puVar5 = local_10;
  uVar12 = local_10[7] >> 5;
  puVar11 = local_18;
  pcVar8 = local_14;
  if (((uVar12 & 1) != 0) && (local_10[9] == 0)) {
    piVar4 = (int *)local_10[2];
    if ((char)piVar4[5] == '\0') {
      FUN_0054a360(piVar4);
      pcVar8 = (char *)((int)puVar5 + 0x2d);
    }
    uVar12 = FUN_00549573((byte *)puVar5[0xc],piVar4 + 3);
    puVar11 = puVar5 + 0xc;
  }
  cVar2 = *pcVar8;
  uVar12 = CONCAT31((int3)(uVar12 >> 8),cVar2);
  if (((cVar2 == 'g') || (cVar2 == 'G')) && (uVar12 = puVar5[7] >> 5, (uVar12 & 1) == 0)) {
    piVar4 = (int *)puVar5[2];
    if ((char)piVar4[5] == '\0') {
      FUN_0054a360(piVar4);
      puVar11 = puVar5 + 0xc;
    }
    uVar12 = FUN_005494cc((char *)puVar5[0xc],piVar4 + 3);
  }
  pcVar8 = (char *)puVar5[0xc];
  cVar2 = *pcVar8;
  if (cVar2 == '-') {
    puVar5[7] = puVar5[7] | 0x40;
    pcVar8 = pcVar8 + 1;
    *puVar11 = (uint)pcVar8;
    cVar2 = *pcVar8;
  }
  if (((cVar2 == 'i') || (cVar2 == 'I')) || ((cVar2 == 'n' || (cVar2 == 'N')))) {
    puVar5[7] = puVar5[7] & 0xfffffff7;
    *(undefined1 *)((int)puVar5 + 0x2d) = 0x73;
  }
  pcVar1 = pcVar8 + 1;
  do {
    cVar2 = *pcVar8;
    pcVar8 = pcVar8 + 1;
  } while (cVar2 != '\0');
  puVar5[0xd] = (int)pcVar8 - (int)pcVar1;
  return CONCAT31((int3)(uVar12 >> 8),1);
}

