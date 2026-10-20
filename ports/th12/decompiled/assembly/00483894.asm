
// block 00483894
00483894  call       0x475467

// block 00483899
00483899  mov        ecx, eax
0048389b  mov        eax, dword ptr [ecx + 0x6c]
0048389e  cmp        eax, dword ptr [0x4adab8]
004838a4  je         0x4838b6

// block 004838a6
004838a6  mov        edx, dword ptr [0x4ad9d4]
004838ac  test       dword ptr [ecx + 0x70], edx
004838af  jne        0x4838b6

// block 004838b1
004838b1  call       0x474181

// block 004838b6
004838b6  mov        eax, dword ptr [eax + 0xc8]
004838bc  ret        
