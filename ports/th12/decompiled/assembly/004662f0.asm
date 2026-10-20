
// block 004662f0
004662f0  mov        eax, dword ptr [ecx + 4]
004662f3  test       eax, eax
004662f5  jne        0x4662fa

// block 004662f7
004662f7  xor        eax, eax
004662f9  ret        

// block 004662fa
004662fa  cmp        dword ptr [ecx + 0x10], 0
004662fe  jbe        0x4662f7

// block 00466300
00466300  mov        eax, dword ptr [eax]
00466302  ret        
