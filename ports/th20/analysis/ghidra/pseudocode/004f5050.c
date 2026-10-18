/* Entry: 0x004f5050; symbol: bad_alloc; body bytes: 38 */

/* Library Function - Single Match
    public: __thiscall std::bad_alloc::bad_alloc(void)
   
   Libraries: Visual Studio 2015, Visual Studio 2017, Visual Studio 2019 */

bad_alloc * __thiscall std::bad_alloc::bad_alloc(bad_alloc *this)

{
  exception::exception((exception *)this,"bad cast",1);
  *(undefined ***)this = &PTR_FID_conflict__scalar_deleting_destructor__00572674;
  return this;
}

