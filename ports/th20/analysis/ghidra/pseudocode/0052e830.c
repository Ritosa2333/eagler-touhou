/* Entry: 0x0052e830; symbol: _Getcont; body bytes: 43 */

/* Library Function - Single Match
    public: struct std::_Container_base12 const * __thiscall
   std::_Iterator_base12::_Getcont(void)const 
   
   Libraries: Visual Studio 2019 Debug, Visual Studio 2019 Release */

_Container_base12 * __thiscall std::_Iterator_base12::_Getcont(_Iterator_base12 *this)

{
  undefined4 local_c;
  
  if (*(int *)this == 0) {
    local_c = (_Container_base12 *)0x0;
  }
  else {
    local_c = (_Container_base12 *)**(undefined4 **)this;
  }
  return local_c;
}

