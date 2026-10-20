
// block 00451c20
00451c20  and        dword ptr [0x4cee78], 0xfffff3ff
00451c2a  mov        eax, 0x4ce8e8
00451c2f  call       0x451a60

// block 00451c34
00451c34  mov        eax, dword ptr [0x4cee78]
00451c39  xor        ecx, ecx
00451c3b  cmp        dword ptr [0x4ce908], ecx
00451c41  setne      cl
00451c44  xor        edx, edx
00451c46  shl        ecx, 0xa
00451c49  xor        ecx, eax
00451c4b  and        ecx, 0x400
00451c51  xor        eax, ecx
00451c53  cmp        dword ptr [0x4ce90c], edx
00451c59  setne      dl
00451c5c  shl        edx, 0xb
00451c5f  xor        edx, eax
00451c61  and        edx, 0x800
00451c67  xor        eax, edx
00451c69  mov        dword ptr [0x4cee78], eax
00451c6e  ret        
