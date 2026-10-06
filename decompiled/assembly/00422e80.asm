
// block 00422e80
00422e80  push       ecx
00422e81  test       edi, edi
00422e83  jle        0x422f15

// block 00422e89
00422e89  cmp        dword ptr [esi + 8], 0
00422e8d  mov        dword ptr [esi + 0x14], 0
00422e94  jne        0x422f15

// block 00422e96
00422e96  mov        ecx, dword ptr [esi]
00422e98  test       ecx, ecx
00422e9a  jne        0x422ead

// block 00422e9c
00422e9c  mov        eax, 1
00422ea1  mov        dword ptr [esi + 0xc], eax
00422ea4  mov        dword ptr [esi + 0x10], eax
00422ea7  mov        dword ptr [esi], edi
00422ea9  xor        eax, eax
00422eab  pop        ecx
00422eac  ret        

// block 00422ead
00422ead  mov        eax, dword ptr [esi + 4]
00422eb0  test       eax, eax
00422eb2  jne        0x422ec6

// block 00422eb4
00422eb4  mov        eax, 2
00422eb9  mov        dword ptr [esi + 0xc], eax
00422ebc  mov        dword ptr [esi + 0x10], eax
00422ebf  mov        dword ptr [esi + 4], edi
00422ec2  xor        eax, eax
00422ec4  pop        ecx
00422ec5  ret        

// block 00422ec6
00422ec6  mov        edx, 3
00422ecb  mov        dword ptr [esi + 8], edi
00422ece  mov        dword ptr [esi + 0xc], edx
00422ed1  mov        dword ptr [esi + 0x10], edx
00422ed4  cmp        ecx, edi
00422ed6  jne        0x422ee3

// block 00422ed8
00422ed8  cmp        eax, edi
00422eda  jne        0x422ef3

// block 00422edc
00422edc  mov        dword ptr [esi + 0x18], edi
00422edf  mov        eax, edi
00422ee1  pop        ecx
00422ee2  ret        

// block 00422ee3
00422ee3  cmp        eax, edi
00422ee5  je         0x422ef3

// block 00422ee7
00422ee7  cmp        ecx, eax
00422ee9  je         0x422ef3

// block 00422eeb
00422eeb  or         eax, 0xffffffff
00422eee  mov        dword ptr [esi + 0x18], eax
00422ef1  pop        ecx
00422ef2  ret        

// block 00422ef3
00422ef3  call       0x421a60

// block 00422ef8
00422ef8  mov        eax, dword ptr [esi + 4]
00422efb  mov        dword ptr [esi], eax
00422efd  mov        dword ptr [esi + 8], 0
00422f04  mov        dword ptr [esi + 0xc], 2
00422f0b  mov        dword ptr [esi + 0x10], 4
00422f12  mov        dword ptr [esi + 4], edi

// block 00422f15
00422f15  xor        eax, eax
00422f17  pop        ecx
00422f18  ret        
