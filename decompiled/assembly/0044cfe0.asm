
// block 0044cfe0
0044cfe0  push       esi
0044cfe1  mov        esi, ecx
0044cfe3  mov        eax, dword ptr [esi + 0xc]
0044cfe6  push       edi
0044cfe7  xor        edi, edi
0044cfe9  mov        dword ptr [esi], 0x4a23d4
0044cfef  cmp        eax, edi
0044cff1  je         0x44cfff

// block 0044cff3
0044cff3  push       eax
0044cff4  call       0x46c941

// block 0044cff9
0044cff9  add        esp, 4
0044cffc  mov        dword ptr [esi + 0xc], edi

// block 0044cfff
0044cfff  mov        dword ptr [esi + 0xc], edi
0044d002  mov        dword ptr [esi + 4], edi
0044d005  mov        dword ptr [esi + 8], edi
0044d008  pop        edi
0044d009  mov        dword ptr [esi], 0x4a2304
0044d00f  pop        esi
0044d010  ret        
