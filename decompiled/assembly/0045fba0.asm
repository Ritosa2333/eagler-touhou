
// block 0045fba0
0045fba0  and        dword ptr [esi + 0x10], 0xfffffffe
0045fba4  mov        eax, dword ptr [edi*4 + 0x4a2338]
0045fbab  mov        ecx, dword ptr [0x4ce8f0]
0045fbb1  push       ebp
0045fbb2  mov        ebp, dword ptr [esp + 8]
0045fbb6  push       esi
0045fbb7  push       1
0045fbb9  push       eax
0045fbba  push       0
0045fbbc  push       1
0045fbbe  push       ebx
0045fbbf  push       ebp
0045fbc0  push       ecx
0045fbc1  call       0x46c6e6

// block 0045fbc6
0045fbc6  mov        edx, dword ptr [edi*4 + 0x4a235c]
0045fbcd  mov        eax, ebp
0045fbcf  imul       eax, ebx
0045fbd2  lea        ecx, [edi*4 + 0x4a235c]
0045fbd9  mov        dword ptr [esi + 0xc], edx
0045fbdc  imul       eax, dword ptr [ecx]
0045fbdf  pop        ebp
0045fbe0  ret        4
