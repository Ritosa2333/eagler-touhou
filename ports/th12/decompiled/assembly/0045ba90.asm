
// block 0045ba90
0045ba90  push       ebx
0045ba91  mov        ebx, eax
0045ba93  push       esi
0045ba94  call       0x45b930

// block 0045ba99
0045ba99  push       0
0045ba9b  mov        eax, ebx
0045ba9d  mov        ecx, esi
0045ba9f  call       0x459e50

// block 0045baa4
0045baa4  fld1       
0045baa6  fst        dword ptr [0x4d4848]
0045baac  pop        ebx
0045baad  fst        dword ptr [0x4d482c]
0045bab3  fst        dword ptr [0x4d4810]
0045bab9  fstp       dword ptr [0x4d47f4]
0045babf  ret        
