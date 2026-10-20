
// block 0041d530
0041d530  push       esi
0041d531  mov        esi, dword ptr [0x4ce8cc]
0041d537  add        esi, 0x4b50d4
0041d53d  push       edi
0041d53e  mov        edi, dword ptr [esi]
0041d540  test       edi, edi
0041d542  je         0x41d55a

// block 0041d544
0041d544  call       0x4604e0

// block 0041d549
0041d549  mov        eax, dword ptr [esi]
0041d54b  push       eax
0041d54c  call       0x46ca4f

// block 0041d551
0041d551  add        esp, 4
0041d554  mov        dword ptr [esi], 0

// block 0041d55a
0041d55a  pop        edi
0041d55b  xor        eax, eax
0041d55d  pop        esi
0041d55e  ret        
