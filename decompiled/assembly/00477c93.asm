
// block 00477c93
00477c93  mov        edi, edi
00477c95  push       ebp
00477c96  mov        ebp, esp
00477c98  mov        eax, 0xe06d7363
00477c9d  cmp        dword ptr [ebp + 8], eax
00477ca0  jne        0x477caf

// block 00477ca2
00477ca2  push       dword ptr [ebp + 0xc]
00477ca5  push       eax
00477ca6  call       0x477b33

// block 00477cab
00477cab  pop        ecx
00477cac  pop        ecx
00477cad  pop        ebp
00477cae  ret        

// block 00477caf
00477caf  xor        eax, eax
00477cb1  pop        ebp
00477cb2  ret        
