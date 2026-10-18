/* Entry: 0x0051b250; symbol: clear; body bytes: 148 */

/* Library Function - Single Match
    public: void __thiscall std::ios_base::clear(int,bool)
   
   Libraries: Visual Studio 2017 Debug, Visual Studio 2017 Release, Visual Studio 2019 Debug, Visual
   Studio 2019 Release */

void __thiscall std::ios_base::clear(ios_base *this,int param_1,bool param_2)

{
  error_code *peVar1;
  failure local_2c [20];
  undefined1 local_18 [8];
  ios_base *local_10;
  char *local_c;
  uint local_8;
  
  *(uint *)(this + 0xc) = param_1 & 0x17U;
  local_8 = param_1 & 0x17U & *(uint *)(this + 0x10);
  if (local_8 != 0) {
    local_10 = this;
    if (param_2) {
      FUN_00544a7c((int *)0x0,(byte *)0x0);
    }
    if ((local_8 & 4) == 0) {
      if ((local_8 & 2) == 0) {
        local_c = "ios_base::eofbit set";
      }
      else {
        local_c = "ios_base::failbit set";
      }
    }
    else {
      local_c = "ios_base::badbit set";
    }
    peVar1 = (error_code *)make_error_code((io_errc)local_18);
    failure::failure(local_2c,local_c,peVar1);
    FUN_00544a7c((int *)local_2c,&DAT_005abb7c);
  }
  return;
}

