
// block 004358c0
004358c0  mov        eax, dword ptr [0x4b44e8]
004358c5  mov        eax, dword ptr [eax + 8]
004358c8  test       eax, eax
004358ca  je         0x4358d8

// block 004358cc
004358cc  test       byte ptr [eax + 4], 2
004358d0  je         0x4358d8

// block 004358d2
004358d2  mov        eax, 1
004358d7  ret        

// block 004358d8
004358d8  xor        eax, eax
004358da  ret        
