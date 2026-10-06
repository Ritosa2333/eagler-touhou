
// block 0044cd30
0044cd30  push       esi
0044cd31  push       edi
0044cd32  mov        edi, dword ptr [esp + 0xc]
0044cd36  mov        esi, ecx
0044cd38  mov        ecx, edi
0044cd3a  call       0x44c230

// block 0044cd3f
0044cd3f  mov        ecx, edi
0044cd41  mov        dword ptr [esi + 0xc], eax
0044cd44  call       0x44c310

// block 0044cd49
0044cd49  mov        dword ptr [esi + 4], eax
0044cd4c  mov        eax, dword ptr [esi + 0xc]
0044cd4f  mov        dword ptr [esi + 8], eax
0044cd52  test       eax, eax
0044cd54  pop        edi
0044cd55  setne      al
0044cd58  pop        esi
0044cd59  ret        8
