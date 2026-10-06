
// block 0048a24a
0048a24a  mov        edi, edi
0048a24c  push       ebp
0048a24d  mov        ebp, esp
0048a24f  push       ecx
0048a250  push       ecx
0048a251  cmp        dword ptr [ebp + 8], 0
0048a255  push       dword ptr [ebp + 0x14]
0048a258  push       dword ptr [ebp + 0x10]
0048a25b  je         0x48a276

// block 0048a25d
0048a25d  lea        eax, [ebp - 8]
0048a260  push       eax
0048a261  call       0x48dc40

// block 0048a266
0048a266  mov        ecx, dword ptr [ebp - 8]
0048a269  mov        eax, dword ptr [ebp + 0xc]
0048a26c  mov        dword ptr [eax], ecx
0048a26e  mov        ecx, dword ptr [ebp - 4]
0048a271  mov        dword ptr [eax + 4], ecx
0048a274  jmp        0x48a287

// block 0048a276
0048a276  lea        eax, [ebp + 8]
0048a279  push       eax
0048a27a  call       0x48ddbf

// block 0048a27f
0048a27f  mov        eax, dword ptr [ebp + 0xc]
0048a282  mov        ecx, dword ptr [ebp + 8]
0048a285  mov        dword ptr [eax], ecx

// block 0048a287
0048a287  add        esp, 0xc
0048a28a  leave      
0048a28b  ret        
