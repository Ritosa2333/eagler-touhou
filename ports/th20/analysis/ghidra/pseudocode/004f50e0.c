/* Entry: 0x004f50e0; symbol: FID_conflict:length_error; body bytes: 37 */

/* Library Function - Multiple Matches With Different Base Names
    public: __thiscall std::length_error::length_error(char const *)
    public: __thiscall std::out_of_range::out_of_range(char const *)
    public: __thiscall std::overflow_error::overflow_error(char const *)
    public: __thiscall std::range_error::range_error(char const *)
   
   Libraries: Visual Studio 2015, Visual Studio 2017, Visual Studio 2019 */

undefined4 * __thiscall FID_conflict_length_error(void *this,int param_1)

{
  FUN_004f5190(this,param_1);
  *(undefined ***)this = &PTR_FID_conflict__scalar_deleting_destructor__005726cc;
  return (undefined4 *)this;
}

