/* Entry: 0x0054929b; symbol: FUN_0054929b; body bytes: 116 */

void __thiscall FUN_0054929b(void *this,uint param_1,uint param_2)

{
  byte bVar1;
  uint uVar2;
  int iVar3;
  formatting_buffer *pfVar4;
  formatting_buffer *pfVar5;
  
  pfVar4 = *(formatting_buffer **)((int)this + 0x440);
  if (*(formatting_buffer **)((int)this + 0x440) == (formatting_buffer *)0x0) {
    pfVar4 = (formatting_buffer *)((int)this + 0x3c);
  }
  uVar2 = __crt_stdio_output::formatting_buffer::count<char>
                    ((formatting_buffer *)((int)this + 0x3c));
  pfVar4 = pfVar4 + (uVar2 - 1);
  *(formatting_buffer **)((int)this + 0x30) = pfVar4;
  pfVar5 = pfVar4;
  do {
    iVar3 = *(int *)((int)this + 0x24);
    if (iVar3 < 1) {
      if (param_1 == 0 && param_2 == 0) {
        *(int *)((int)this + 0x34) = (int)pfVar4 - (int)pfVar5;
        *(formatting_buffer **)((int)this + 0x30) = pfVar5 + 1;
        return;
      }
      iVar3 = *(int *)((int)this + 0x24);
    }
    *(int *)((int)this + 0x24) = iVar3 + -1;
    bVar1 = (byte)param_1;
    param_1 = param_1 >> 3 | param_2 << 0x1d;
    *pfVar5 = (formatting_buffer)((bVar1 & 7) + 0x30);
    param_2 = param_2 >> 3;
    *(int *)((int)this + 0x30) = *(int *)((int)this + 0x30) + -1;
    pfVar5 = *(formatting_buffer **)((int)this + 0x30);
  } while( true );
}

