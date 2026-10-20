
// block 0046fd50
0046fd50  mov        eax, dword ptr [edi]
0046fd52  cmp        eax, -2
0046fd55  je         0x46fd64

// block 0046fd57
0046fd57  mov        ecx, dword ptr [edi + 4]
0046fd5a  add        ecx, esi
0046fd5c  xor        ecx, dword ptr [eax + esi]
0046fd5f  call       0x46c932

// block 0046fd64
0046fd64  mov        ecx, dword ptr [edi + 0xc]
0046fd67  mov        eax, dword ptr [edi + 8]
0046fd6a  add        ecx, esi
0046fd6c  xor        ecx, dword ptr [eax + esi]
0046fd6f  jmp        0x46c932
