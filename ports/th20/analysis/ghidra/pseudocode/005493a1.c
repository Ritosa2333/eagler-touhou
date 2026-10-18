/* Entry: 0x005493a1; symbol: FUN_005493a1; body bytes: 134 */

void __thiscall FUN_005493a1(void *this,uint param_1,uint param_2,byte param_3)

{
  byte bVar1;
  formatting_buffer fVar2;
  uint uVar3;
  int iVar4;
  formatting_buffer *pfVar5;
  formatting_buffer *pfVar6;
  
  pfVar5 = *(formatting_buffer **)((int)this + 0x440);
  if (*(formatting_buffer **)((int)this + 0x440) == (formatting_buffer *)0x0) {
    pfVar5 = (formatting_buffer *)((int)this + 0x3c);
  }
  uVar3 = __crt_stdio_output::formatting_buffer::count<char>
                    ((formatting_buffer *)((int)this + 0x3c));
  pfVar5 = pfVar5 + (uVar3 - 1);
  *(formatting_buffer **)((int)this + 0x30) = pfVar5;
  pfVar6 = pfVar5;
  do {
    iVar4 = *(int *)((int)this + 0x24);
    if (iVar4 < 1) {
      if (param_1 == 0 && param_2 == 0) {
        *(int *)((int)this + 0x34) = (int)pfVar5 - (int)pfVar6;
        *(formatting_buffer **)((int)this + 0x30) = pfVar6 + 1;
        return;
      }
      iVar4 = *(int *)((int)this + 0x24);
    }
    *(int *)((int)this + 0x24) = iVar4 + -1;
    bVar1 = (byte)param_1;
    param_1 = param_1 >> 4 | param_2 << 0x1c;
    fVar2 = (formatting_buffer)((bVar1 & 0xf) + 0x30);
    param_2 = param_2 >> 4;
    if (0x39 < (byte)fVar2) {
      fVar2 = (formatting_buffer)((param_3 ^ 1) * ' ' + '\a' + (char)fVar2);
    }
    *pfVar6 = fVar2;
    *(int *)((int)this + 0x30) = *(int *)((int)this + 0x30) + -1;
    pfVar6 = *(formatting_buffer **)((int)this + 0x30);
  } while( true );
}

