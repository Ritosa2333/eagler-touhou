/* Entry: 0x0054930f; symbol: FUN_0054930f; body bytes: 146 */

void __thiscall FUN_0054930f(void *this,undefined4 param_1,undefined4 param_2,byte param_3)

{
  uint uVar1;
  char extraout_CL;
  formatting_buffer fVar2;
  formatting_buffer *pfVar3;
  formatting_buffer *pfVar4;
  longlong lVar5;
  
  pfVar4 = *(formatting_buffer **)((int)this + 0x440);
  if (*(formatting_buffer **)((int)this + 0x440) == (formatting_buffer *)0x0) {
    pfVar4 = (formatting_buffer *)((int)this + 0x3c);
  }
  uVar1 = __crt_stdio_output::formatting_buffer::count<char>
                    ((formatting_buffer *)((int)this + 0x3c));
  pfVar4 = pfVar4 + (uVar1 - 1);
  *(formatting_buffer **)((int)this + 0x30) = pfVar4;
  lVar5 = CONCAT44(param_2,param_1);
  pfVar3 = pfVar4;
  while( true ) {
    if ((*(int *)((int)this + 0x24) < 1) && (lVar5 == 0)) break;
    *(int *)((int)this + 0x24) = *(int *)((int)this + 0x24) + -1;
    lVar5 = __aulldvrm((uint)lVar5,(uint)((ulonglong)lVar5 >> 0x20),10,0);
    fVar2 = (formatting_buffer)(extraout_CL + '0');
    if ('9' < (char)fVar2) {
      fVar2 = (formatting_buffer)((char)fVar2 + (param_3 ^ 1) * ' ' + '\a');
    }
    *pfVar3 = fVar2;
    *(int *)((int)this + 0x30) = *(int *)((int)this + 0x30) + -1;
    pfVar3 = *(formatting_buffer **)((int)this + 0x30);
  }
  *(int *)((int)this + 0x34) = (int)pfVar4 - (int)pfVar3;
  *(formatting_buffer **)((int)this + 0x30) = pfVar3 + 1;
  return;
}

