
// block 0044ccf0
0044ccf0  push       esi
0044ccf1  mov        esi, ecx
0044ccf3  mov        eax, dword ptr [esi + 0xc]
0044ccf6  push       edi
0044ccf7  xor        edi, edi
0044ccf9  mov        dword ptr [esi], 0x4a23d4
0044ccff  cmp        eax, edi
0044cd01  je         0x44cd0f

// block 0044cd03
0044cd03  push       eax
0044cd04  call       0x46c941

// block 0044cd09
0044cd09  add        esp, 4
0044cd0c  mov        dword ptr [esi + 0xc], edi

// block 0044cd0f
0044cd0f  mov        dword ptr [esi + 0xc], edi
0044cd12  mov        dword ptr [esi + 4], edi
0044cd15  mov        dword ptr [esi + 8], edi
0044cd18  pop        edi
0044cd19  mov        dword ptr [esi], 0x4a2304
0044cd1f  pop        esi
0044cd20  ret        
