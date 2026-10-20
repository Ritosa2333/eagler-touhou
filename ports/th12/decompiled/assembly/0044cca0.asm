
// block 0044cca0
0044cca0  push       esi
0044cca1  mov        esi, ecx
0044cca3  mov        eax, dword ptr [esi + 0xc]
0044cca6  push       edi
0044cca7  xor        edi, edi
0044cca9  mov        dword ptr [esi], 0x4a23d4
0044ccaf  cmp        eax, edi
0044ccb1  je         0x44ccbf

// block 0044ccb3
0044ccb3  push       eax
0044ccb4  call       0x46c941

// block 0044ccb9
0044ccb9  add        esp, 4
0044ccbc  mov        dword ptr [esi + 0xc], edi

// block 0044ccbf
0044ccbf  test       byte ptr [esp + 0xc], 1
0044ccc4  mov        dword ptr [esi + 0xc], edi
0044ccc7  mov        dword ptr [esi + 4], edi
0044ccca  mov        dword ptr [esi + 8], edi
0044cccd  mov        dword ptr [esi], 0x4a2304
0044ccd3  je         0x44ccde

// block 0044ccd5
0044ccd5  push       esi
0044ccd6  call       0x46ca4f

// block 0044ccdb
0044ccdb  add        esp, 4

// block 0044ccde
0044ccde  pop        edi
0044ccdf  mov        eax, esi
0044cce1  pop        esi
0044cce2  ret        4
