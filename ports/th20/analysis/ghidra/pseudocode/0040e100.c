/* Entry: 0x0040e100; symbol: bad_array_new_length; body bytes: 36 */

/* Library Function - Single Match
    public: __thiscall std::bad_array_new_length::bad_array_new_length(void)
   
   Libraries: Visual Studio 2015 Debug, Visual Studio 2017 Debug, Visual Studio 2019 Debug, Visual
   Studio 2019 Release */

bad_array_new_length * __thiscall
std::bad_array_new_length::bad_array_new_length(bad_array_new_length *this)

{
  bad_alloc::bad_alloc((bad_alloc *)this,"bad array new length");
  *(undefined ***)this = &PTR_FID_conflict__scalar_deleting_destructor__0056c4a8;
  return this;
}

