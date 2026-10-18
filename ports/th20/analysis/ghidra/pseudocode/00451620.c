/* Entry: 0x00451620; symbol: operator_union_tagCY; body bytes: 32 */

/* Library Function - Single Match
    public: __thiscall COleCurrency::operator union tagCY(void)const 
   
   Libraries: Visual Studio 2003 Debug, Visual Studio 2005 Debug, Visual Studio 2008 Debug, Visual
   Studio 2010 Debug */

tagCY __thiscall COleCurrency::operator_union_tagCY(COleCurrency *this)

{
  undefined4 uVar1;
  tagCY tVar2;
  undefined4 *in_stack_00000004;
  
  uVar1 = *(undefined4 *)(this + 4);
  *in_stack_00000004 = *(undefined4 *)this;
  in_stack_00000004[1] = uVar1;
  tVar2.s.Hi = uVar1;
  tVar2.s.Lo = (ulong)in_stack_00000004;
  return (tagCY)tVar2.int64;
}

