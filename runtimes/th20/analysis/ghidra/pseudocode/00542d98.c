/* Entry: 0x00542d98; symbol: dtol3_getbits; body bytes: 54 */

/* Library Function - Single Match
    _dtol3_getbits
   
   Libraries: Visual Studio 2019 Debug, Visual Studio 2019 Release */

undefined8 __cdecl dtol3_getbits(void)

{
  byte bVar1;
  uint in_EAX;
  byte bVar2;
  uint in_EDX;
  uint uVar3;
  
  uVar3 = in_EDX & 0x1fffff | 0x100000;
  bVar2 = (char)(in_EDX >> 0x14) - 0x33;
  if (in_EDX >> 0x14 < 0x433) {
    bVar1 = -bVar2 & 0x1f;
    return CONCAT44(uVar3 >> (-bVar2 & 0x1f),in_EAX >> bVar1 | uVar3 << 0x20 - bVar1);
  }
  return CONCAT44(uVar3 << (bVar2 & 0x1f) | in_EAX >> 0x20 - (bVar2 & 0x1f),in_EAX << (bVar2 & 0x1f)
                 );
}

