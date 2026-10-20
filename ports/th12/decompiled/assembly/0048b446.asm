
// block 0048b446
0048b446  mov        eax, dword ptr [ebp - 0x14]
0048b449  mov        eax, dword ptr [eax]
0048b44b  mov        eax, dword ptr [eax]
0048b44d  cmp        eax, 0xc0000005
0048b452  je         0x48b45e

// block 0048b454
0048b454  cmp        eax, 0xc000001d
0048b459  je         0x48b45e

// block 0048b45b
0048b45b  xor        eax, eax
0048b45d  ret        

// block 0048b45e
0048b45e  xor        eax, eax
0048b460  inc        eax
0048b461  ret        
