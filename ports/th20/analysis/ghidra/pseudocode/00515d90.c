/* Entry: 0x00515d90; symbol: failure; body bytes: 47 */

/* Library Function - Single Match
    public: __thiscall std::ios_base::failure::failure(char const *,class std::error_code const &)
   
   Libraries: Visual Studio 2017 Debug, Visual Studio 2017 Release, Visual Studio 2019 Debug, Visual
   Studio 2019 Release */

failure * __thiscall
std::ios_base::failure::failure(failure *this,char *param_1,error_code *param_2)

{
  FUN_00515f90(*(undefined4 *)param_2,*(undefined4 *)(param_2 + 4),(uint *)param_1);
  *(undefined ***)this = &PTR_FID_conflict__scalar_deleting_destructor__00574d1c;
  return this;
}

