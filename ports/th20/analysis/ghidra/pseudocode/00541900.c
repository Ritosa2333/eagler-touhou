/* Entry: 0x00541900; symbol: __Xtime_get_ticks; body bytes: 39 */

/* Library Function - Single Match
    __Xtime_get_ticks
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

undefined8 __Xtime_get_ticks(void)

{
  _FILETIME local_c;
  
  FUN_0054242b(&local_c);
  return CONCAT44((local_c.dwHighDateTime + 0xfe624e22) - (uint)(local_c.dwLowDateTime < 0xd53e8000)
                  ,local_c.dwLowDateTime + 0x2ac18000);
}

