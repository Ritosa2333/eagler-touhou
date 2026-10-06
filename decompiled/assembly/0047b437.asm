
// block 0047b437
0047b437  mov        eax, dword ptr [ebp - 0x14]
0047b43a  mov        eax, dword ptr [eax]
0047b43c  mov        eax, dword ptr [eax]
0047b43e  mov        dword ptr [ebp - 0x20], eax
0047b441  xor        ecx, ecx
0047b443  cmp        eax, 0xc0000017
0047b448  sete       cl
0047b44b  mov        eax, ecx
0047b44d  ret        
