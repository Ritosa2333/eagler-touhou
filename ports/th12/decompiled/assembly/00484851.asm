
// block 00484851
00484851  call       0x475467

// block 00484856
00484856  mov        ecx, eax
00484858  mov        eax, dword ptr [ecx + 0x6c]
0048485b  cmp        eax, dword ptr [0x4adab8]
00484861  je         0x484873

// block 00484863
00484863  mov        edx, dword ptr [0x4ad9d4]
00484869  test       dword ptr [ecx + 0x70], edx
0048486c  jne        0x484873

// block 0048486e
0048486e  call       0x474181

// block 00484873
00484873  mov        eax, dword ptr [eax + 0xac]
00484879  ret        
