
// block 0046ff8f
0046ff8f  mov        edi, edi
0046ff91  push       ebp
0046ff92  mov        ebp, esp
0046ff94  sub        esp, 0x20
0046ff97  mov        eax, dword ptr [ebp + 8]
0046ff9a  push       esi
0046ff9b  push       edi
0046ff9c  push       8
0046ff9e  pop        ecx
0046ff9f  mov        esi, 0x49cd70
0046ffa4  lea        edi, [ebp - 0x20]

// block 0046ffa7
0046ffa7  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0046ffa9
0046ffa9  mov        dword ptr [ebp - 8], eax
0046ffac  mov        eax, dword ptr [ebp + 0xc]
0046ffaf  pop        edi
0046ffb0  mov        dword ptr [ebp - 4], eax
0046ffb3  pop        esi
0046ffb4  test       eax, eax
0046ffb6  je         0x46ffc4

// block 0046ffb8
0046ffb8  test       byte ptr [eax], 8
0046ffbb  je         0x46ffc4

// block 0046ffbd
0046ffbd  mov        dword ptr [ebp - 0xc], 0x1994000

// block 0046ffc4
0046ffc4  lea        eax, [ebp - 0xc]
0046ffc7  push       eax
0046ffc8  push       dword ptr [ebp - 0x10]
0046ffcb  push       dword ptr [ebp - 0x1c]
0046ffce  push       dword ptr [ebp - 0x20]
0046ffd1  call       dword ptr [0x498178]

// block 0046ffd7
0046ffd7  leave      
0046ffd8  ret        8
