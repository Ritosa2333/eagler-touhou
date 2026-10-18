/* Entry: 0x0054913e; symbol: FUN_0054913e; body bytes: 98 */

void __thiscall FUN_0054913e(void *this,uint param_1)

{
  uint uVar1;
  formatting_buffer *pfVar2;
  formatting_buffer *pfVar3;
  
  pfVar3 = *(formatting_buffer **)((int)this + 0x440);
  if (*(formatting_buffer **)((int)this + 0x440) == (formatting_buffer *)0x0) {
    pfVar3 = (formatting_buffer *)((int)this + 0x3c);
  }
  uVar1 = __crt_stdio_output::formatting_buffer::count<char>
                    ((formatting_buffer *)((int)this + 0x3c));
  pfVar3 = pfVar3 + (uVar1 - 1);
  *(formatting_buffer **)((int)this + 0x30) = pfVar3;
  pfVar2 = pfVar3;
  for (; (0 < *(int *)((int)this + 0x24) || (param_1 != 0)); param_1 = param_1 >> 3) {
    *(int *)((int)this + 0x24) = *(int *)((int)this + 0x24) + -1;
    *pfVar2 = (formatting_buffer)(((byte)param_1 & 7) + 0x30);
    *(int *)((int)this + 0x30) = *(int *)((int)this + 0x30) + -1;
    pfVar2 = *(formatting_buffer **)((int)this + 0x30);
  }
  *(int *)((int)this + 0x34) = (int)pfVar3 - (int)pfVar2;
  *(formatting_buffer **)((int)this + 0x30) = pfVar2 + 1;
  return;
}

