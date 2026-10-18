/* Entry: 0x0040e070; symbol: bad_alloc; body bytes: 39 */

/* Library Function - Single Match
    private: __thiscall std::bad_alloc::bad_alloc(char const * const)
   
   Libraries: Visual Studio 2015 Debug, Visual Studio 2017 Debug, Visual Studio 2019 Debug, Visual
   Studio 2019 Release */

bad_alloc * __thiscall std::bad_alloc::bad_alloc(bad_alloc *this,char *param_1)

{
  exception::exception((exception *)this,param_1,1);
  *(undefined ***)this = &PTR_FID_conflict__scalar_deleting_destructor__0056c49c;
  return this;
}

