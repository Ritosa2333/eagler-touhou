/* Entry: 0x0054a14b; symbol: FUN_0054a14b; body bytes: 140 */

undefined4 __fastcall FUN_0054a14b(undefined6 *param_1)

{
  formatting_buffer *this;
  WCHAR WVar1;
  formatting_buffer *pfVar2;
  bool bVar3;
  uint uVar4;
  int iVar5;
  formatting_buffer *pfVar6;
  
  this = (formatting_buffer *)((int)param_1 + 0x3c);
  bVar3 = __crt_stdio_output::is_wide_character_specifier<char>
                    (CONCAT26((short)((uint)*(undefined4 *)((int)param_1 + 4) >> 0x10),*param_1),
                     *(char *)((int)param_1 + 0x2d),*(length_modifier *)(param_1 + 5));
  pfVar2 = *(formatting_buffer **)(param_1 + 2);
  pfVar6 = pfVar2 + 4;
  if (bVar3) {
    *(formatting_buffer **)(param_1 + 2) = pfVar6;
    WVar1 = *(WCHAR *)pfVar2;
    uVar4 = __crt_stdio_output::formatting_buffer::count<char>(this);
    pfVar6 = *(formatting_buffer **)(param_1 + 0x88);
    if (*(formatting_buffer **)(param_1 + 0x88) == (formatting_buffer *)0x0) {
      pfVar6 = this;
    }
    iVar5 = FUN_005590fc(pfVar6,(int *)((int)param_1 + 0x34),(byte *)pfVar6,uVar4,WVar1,
                         *(int **)(param_1 + 1));
    if (iVar5 != 0) {
      *(undefined1 *)((int)param_1 + 0x2c) = 1;
    }
  }
  else {
    *(formatting_buffer **)(param_1 + 2) = pfVar6;
    iVar5 = CONCAT31((int3)((uint)pfVar6 >> 8),*pfVar2);
    pfVar6 = *(formatting_buffer **)(param_1 + 0x88);
    if (*(formatting_buffer **)(param_1 + 0x88) == (formatting_buffer *)0x0) {
      pfVar6 = this;
    }
    *pfVar6 = *pfVar2;
    *(undefined4 *)((int)param_1 + 0x34) = 1;
  }
  pfVar6 = *(formatting_buffer **)(param_1 + 0x88);
  if (*(formatting_buffer **)(param_1 + 0x88) == (formatting_buffer *)0x0) {
    pfVar6 = this;
  }
  *(formatting_buffer **)(param_1 + 6) = pfVar6;
  return CONCAT31((int3)((uint)iVar5 >> 8),1);
}

