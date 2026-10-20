
// block 0048b479
0048b479  mov        edi, edi
0048b47b  push       ebp
0048b47c  mov        ebp, esp
0048b47e  sub        esp, 0x18
0048b481  xor        eax, eax
0048b483  push       ebx
0048b484  mov        dword ptr [ebp - 4], eax
0048b487  mov        dword ptr [ebp - 0xc], eax
0048b48a  mov        dword ptr [ebp - 8], eax
0048b48d  push       ebx
0048b48e  pushfd     
0048b48f  pop        eax
0048b490  mov        ecx, eax
0048b492  xor        eax, 0x200000
0048b497  push       eax
0048b498  popfd      
0048b499  pushfd     
0048b49a  pop        edx
0048b49b  sub        edx, ecx
0048b49d  je         0x48b4be

// block 0048b49f
0048b49f  push       ecx
0048b4a0  popfd      
0048b4a1  xor        eax, eax
0048b4a3  cpuid      
0048b4a5  mov        dword ptr [ebp - 0xc], eax
0048b4a8  mov        dword ptr [ebp - 0x18], ebx
0048b4ab  mov        dword ptr [ebp - 0x14], edx
0048b4ae  mov        dword ptr [ebp - 0x10], ecx
0048b4b1  mov        eax, 1
0048b4b6  cpuid      
0048b4b8  mov        dword ptr [ebp - 4], edx
0048b4bb  mov        dword ptr [ebp - 8], eax

// block 0048b4be
0048b4be  pop        ebx
0048b4bf  test       dword ptr [ebp - 4], 0x4000000
0048b4c6  je         0x48b4d6

// block 0048b4c8
0048b4c8  call       0x48b429

// block 0048b4cd
0048b4cd  test       eax, eax
0048b4cf  je         0x48b4d6

// block 0048b4d1
0048b4d1  xor        eax, eax
0048b4d3  inc        eax
0048b4d4  jmp        0x48b4d8

// block 0048b4d6
0048b4d6  xor        eax, eax

// block 0048b4d8
0048b4d8  pop        ebx
0048b4d9  leave      
0048b4da  ret        
