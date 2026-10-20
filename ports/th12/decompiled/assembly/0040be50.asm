
// block 0040be50
0040be50  fldz       
0040be52  fcomp      dword ptr [ecx + 0x4c0]
0040be58  fnstsw     ax
0040be5a  test       ah, 0x41
0040be5d  jne        0x40be8a

// block 0040be5f
0040be5f  test       byte ptr [ecx + 0x800], 0x10
0040be66  jne        0x40be84

// block 0040be68
0040be68  fld        dword ptr [ecx + 0x4d8]
0040be6e  fchs       
0040be70  fstp       dword ptr [ecx + 0x4d8]
0040be76  fld        dword ptr [ecx + 0x4c0]
0040be7c  fchs       
0040be7e  fstp       dword ptr [ecx + 0x4c0]

// block 0040be84
0040be84  mov        eax, 1
0040be89  ret        

// block 0040be8a
0040be8a  xor        eax, eax
0040be8c  ret        
