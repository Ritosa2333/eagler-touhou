/* Entry: 0x00548c79; symbol: scratch_data<char>; body bytes: 30 */

/* Library Function - Single Match
    public: char * __thiscall __crt_stdio_output::formatting_buffer::scratch_data<char>(void)
   
   Libraries: Visual Studio 2015, Visual Studio 2017, Visual Studio 2019 */

char * __thiscall __crt_stdio_output::formatting_buffer::scratch_data<char>(formatting_buffer *this)

{
  int iVar1;
  uint uVar2;
  formatting_buffer *pfVar3;
  
  iVar1 = *(int *)(this + 0x404);
  uVar2 = count<char>(this);
  if (iVar1 == 0) {
    pfVar3 = this + uVar2;
  }
  else {
    pfVar3 = (formatting_buffer *)(uVar2 + iVar1);
  }
  return (char *)pfVar3;
}

