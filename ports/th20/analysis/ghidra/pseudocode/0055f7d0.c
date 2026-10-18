/* Entry: 0x0055f7d0; symbol: is_contraction_possible; body bytes: 62 */

/* Library Function - Single Match
    bool __cdecl is_contraction_possible(unsigned int)
   
   Library: Visual Studio 2019 Release */

bool __cdecl is_contraction_possible(uint param_1)

{
  BOOL BVar1;
  int local_8;
  
  if (0x4000 < param_1) {
    return true;
  }
  local_8 = -1;
  BVar1 = HeapQueryInformation(DAT_005e5990,HeapCompatibilityInformation,&local_8,4,(PSIZE_T)0x0);
  if (BVar1 == 0) {
    return false;
  }
  return local_8 != 2;
}

