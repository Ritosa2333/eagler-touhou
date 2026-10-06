
// block 004592b0
004592b0  mov        ecx, dword ptr [eax + 0x20]
004592b3  fldz       
004592b5  xor        edx, edx
004592b7  test       cl, 1
004592ba  jne        0x4592d6

// block 004592bc
004592bc  or         ecx, 1
004592bf  fst        dword ptr [eax + 0x18]
004592c2  mov        dword ptr [eax + 0x14], edx
004592c5  mov        dword ptr [eax + 0x10], 0xfff0bdc1
004592cc  mov        dword ptr [eax + 0x1c], 0x4b2ed0
004592d3  mov        dword ptr [eax + 0x20], ecx

// block 004592d6
004592d6  fstp       dword ptr [eax + 0x18]
004592d9  mov        dword ptr [eax + 0x14], edx
004592dc  mov        dword ptr [eax + 0x10], 0xffffffff
004592e3  ret        
