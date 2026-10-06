
// block 004db77b
004db77b  ja         0x4db70a

// block 004db77d
004db77d  pop        ebx
004db77e  xlatb      
004db77f  int        0x27

// block 004db781
004db781  xor        esi, dword ptr [ecx]
004db783  jae        0x4db746

// block 004db785
004db785  sbb        eax, 0xefe735f2
