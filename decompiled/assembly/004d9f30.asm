
// block 004d9f30
004d9f30  adc        esi, dword ptr [eax]
004d9f32  pop        edi
004d9f33  dec        ebx
004d9f34  push       es
004d9f35  call       0x6dab8ac

// block 004d9f3b
004d9f3b  fucomi     st(1)
004d9f3d  mov        dword ptr [ebx], esp
004d9f3f  dec        ecx
004d9f40  dec        edx
004d9f41  mov        edx, 0x79cc2a6a

// block 004d9f46
004d9f46  lea        esp, [ebx]
