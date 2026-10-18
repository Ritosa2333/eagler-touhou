/* Entry: 0x0054c620; symbol: as_float; body bytes: 36 */

/* Library Function - Single Match
    public: float & __thiscall __crt_strtox::floating_point_value::as_float(void)const 
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

float * __thiscall __crt_strtox::floating_point_value::as_float(floating_point_value *this)

{
  if (this[4] == (floating_point_value)0x0) {
    return *(float **)this;
  }
                    /* WARNING: Subroutine does not return */
  __invoke_watson(L"!_is_double",L"__crt_strtox::floating_point_value::as_float",
                  L"minkernel\\crts\\ucrt\\inc\\corecrt_internal_strtox.h",0x1e1,0);
}

