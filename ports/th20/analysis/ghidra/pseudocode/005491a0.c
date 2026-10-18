/* Entry: 0x005491a0; symbol: FUN_005491a0; body bytes: 127 */

void __thiscall FUN_005491a0(void *this,uint param_1,byte param_2)

{
  uint uVar1;
  formatting_buffer *pfVar2;
  formatting_buffer fVar3;
  formatting_buffer *pfVar4;
  
  pfVar4 = *(formatting_buffer **)((int)this + 0x440);
  if (*(formatting_buffer **)((int)this + 0x440) == (formatting_buffer *)0x0) {
    pfVar4 = (formatting_buffer *)((int)this + 0x3c);
  }
  uVar1 = __crt_stdio_output::formatting_buffer::count<char>
                    ((formatting_buffer *)((int)this + 0x3c));
  pfVar4 = pfVar4 + (uVar1 - 1);
  *(formatting_buffer **)((int)this + 0x30) = pfVar4;
  pfVar2 = pfVar4;
  while ((0 < *(int *)((int)this + 0x24) || (param_1 != 0))) {
    *(int *)((int)this + 0x24) = *(int *)((int)this + 0x24) + -1;
    fVar3 = (formatting_buffer)((char)(param_1 % 10) + '0');
    if (0x39 < (byte)fVar3) {
      fVar3 = (formatting_buffer)((char)fVar3 + (param_2 ^ 1) * ' ' + '\a');
    }
    *pfVar2 = fVar3;
    *(int *)((int)this + 0x30) = *(int *)((int)this + 0x30) + -1;
    pfVar2 = *(formatting_buffer **)((int)this + 0x30);
    param_1 = param_1 / 10;
  }
  *(int *)((int)this + 0x34) = (int)pfVar4 - (int)pfVar2;
  *(formatting_buffer **)((int)this + 0x30) = pfVar2 + 1;
  return;
}

