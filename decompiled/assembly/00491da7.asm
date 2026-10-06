
// block 00491da7
00491da7  mov        edi, edi
00491da9  push       ebp
00491daa  mov        ebp, esp
00491dac  push       esi
00491dad  call       0x475467

// block 00491db2
00491db2  mov        esi, dword ptr [ebp + 8]
00491db5  cmp        esi, dword ptr [eax + 0x98]
00491dbb  jne        0x491dce

// block 00491dbd
00491dbd  call       0x475467

// block 00491dc2
00491dc2  mov        ecx, dword ptr [esi + 4]
00491dc5  mov        dword ptr [eax + 0x98], ecx

// block 00491dcb
00491dcb  pop        esi
00491dcc  pop        ebp
00491dcd  ret        

// block 00491dce
00491dce  call       0x475467

// block 00491dd3
00491dd3  mov        eax, dword ptr [eax + 0x98]
00491dd9  jmp        0x491de4

// block 00491ddb
00491ddb  mov        ecx, dword ptr [eax + 4]
00491dde  cmp        esi, ecx
00491de0  je         0x491df1

// block 00491de2
00491de2  mov        eax, ecx

// block 00491de4
00491de4  cmp        dword ptr [eax + 4], 0
00491de8  jne        0x491ddb

// block 00491dea
00491dea  pop        esi
00491deb  pop        ebp
00491dec  jmp        0x472b94

// block 00491df1
00491df1  mov        ecx, dword ptr [esi + 4]
00491df4  mov        dword ptr [eax + 4], ecx
00491df7  jmp        0x491dcb
