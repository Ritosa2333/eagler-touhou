/* Entry: 0x00411590; symbol: push_back; body bytes: 127 */

/* Library Function - Single Match
    public: void __thiscall std::basic_string<char,struct std::char_traits<char>,class
   std::allocator<char> >::push_back(char)
   
   Library: Visual Studio 2019 Release */

void __thiscall
std::basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_>::push_back
          (basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_> *this,
          char param_1)

{
  uint uVar1;
  undefined4 *puVar2;
  undefined1 local_5;
  
  uVar1 = *(uint *)(this + 0x10);
  if (uVar1 < *(uint *)(this + 0x14)) {
    *(uint *)(this + 0x10) = uVar1 + 1;
    puVar2 = Myptr((undefined4 *)this);
    FUN_0040fe90((undefined1 *)((int)puVar2 + uVar1),&param_1);
    local_5 = 0;
    FUN_0040fe90((undefined1 *)((int)puVar2 + uVar1 + 1),&local_5);
  }
  else {
    FUN_0040d4b0(this,1);
  }
  return;
}

