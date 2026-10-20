
// block 0040fe30
0040fe30  push       ebx
0040fe31  push       esi
0040fe32  mov        esi, dword ptr [edi + 0x478]
0040fe38  call       0x464d50

// block 0040fe3d
0040fe3d  mov        ebx, esi
0040fe3f  call       0x464db0

// block 0040fe44
0040fe44  mov        eax, dword ptr [esi]
0040fe46  mov        dword ptr [edi + 0x430], eax
0040fe4c  mov        ecx, dword ptr [esi + 4]
0040fe4f  mov        dword ptr [edi + 0x434], ecx
0040fe55  mov        edx, dword ptr [esi + 8]
0040fe58  pop        esi
0040fe59  mov        dword ptr [edi + 0x438], edx
0040fe5f  xor        eax, eax
0040fe61  pop        ebx
0040fe62  ret        
