
// block 00451d20
00451d20  and        dword ptr [0x4cee78], 0xfffff3ff
00451d2a  mov        eax, 0x4ce8e8
00451d2f  call       0x451a60

// block 00451d34
00451d34  mov        eax, dword ptr [0x4cee78]
00451d39  xor        ecx, ecx
00451d3b  cmp        dword ptr [0x4ce908], ecx
00451d41  setne      cl
00451d44  xor        edx, edx
00451d46  shl        ecx, 0xa
00451d49  xor        ecx, eax
00451d4b  and        ecx, 0x400
00451d51  xor        eax, ecx
00451d53  cmp        dword ptr [0x4ce90c], edx
00451d59  setne      dl
00451d5c  shl        edx, 0xb
00451d5f  xor        edx, eax
00451d61  and        edx, 0x800
00451d67  xor        eax, edx
00451d69  mov        dword ptr [0x4cee78], eax
00451d6e  ret        
