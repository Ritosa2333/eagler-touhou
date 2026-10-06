
// block 004d9f6b
004d9f6b  cmp        edx, ebx
004d9f6e  mov        dword ptr [0x78263ba2], eax
004d9f73  push       edi

// block 004d9f74
004d9f74  loope      0x4d9fe3

// block 004d9f76
004d9f76  dec        eax
004d9f77  popfd      
004d9f78  cwde       
004d9f79  add        dword ptr [edi + 0x287e08ad], ebp
004d9f7f  test       dword ptr [eax], ebx
004d9f81  mov        edx, ebx
004d9f83  jne        0x4d9f46

// block 004d9f85
004d9f85  adc        dword ptr [esi + 0x562f3b20], edx
004d9f8b  inc        esp
004d9f8c  inc        ecx
004d9f8d  sbb        eax, 0x80dad68c
004d9f92  inc        esp
004d9f93  in         eax, dx
004d9f94  ret        0x408b

// block 004d9fe3
004d9fe3  xor        bh, byte ptr [ebx]
004d9fe5  mov        dword ptr [0x186bc225], eax
004d9fea  sti        
