
// block 0040be90
0040be90  fld        dword ptr [ecx + 0x4c0]
0040be96  fld        qword ptr [0x4a3d50]
0040be9c  fcom       st(1)
0040be9e  fnstsw     ax
0040bea0  fstp       st(1)
0040bea2  test       ah, 0x41
0040bea5  jp         0x40bedc

// block 0040bea7
0040bea7  test       byte ptr [ecx + 0x800], 0x10
0040beae  jne        0x40bed4

// block 0040beb0
0040beb0  fld        dword ptr [ecx + 0x4d8]
0040beb6  mov        eax, 1
0040bebb  fchs       
0040bebd  fstp       dword ptr [ecx + 0x4d8]
0040bec3  fld        dword ptr [ecx + 0x4c0]
0040bec9  fsubr      st(1)
0040becb  faddp      st(1)
0040becd  fstp       dword ptr [ecx + 0x4c0]
0040bed3  ret        

// block 0040bed4
0040bed4  fstp       st(0)
0040bed6  mov        eax, 1
0040bedb  ret        

// block 0040bedc
0040bedc  fstp       st(0)
0040bede  xor        eax, eax
0040bee0  ret        
