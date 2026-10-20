
// block 0043c730
0043c730  push       ebx
0043c731  push       esi
0043c732  mov        esi, dword ptr [0x4b4518]
0043c738  mov        eax, dword ptr [esi + 0x10]
0043c73b  push       edi
0043c73c  xor        edi, edi
0043c73e  cmp        eax, edi
0043c740  jne        0x43c7af

// block 0043c742
0043c742  push       0xa0
0043c747  call       0x46c9ea

// block 0043c74c
0043c74c  mov        ebx, eax
0043c74e  add        esp, 4
0043c751  cmp        ebx, edi
0043c753  je         0x43c766

// block 0043c755
0043c755  push       0xa0
0043c75a  push       edi
0043c75b  push       ebx
0043c75c  call       0x477420

// block 0043c761
0043c761  add        esp, 0xc
0043c764  jmp        0x43c768

// block 0043c766
0043c766  xor        ebx, ebx

// block 0043c768
0043c768  mov        eax, dword ptr [0x4b0cb0]
0043c76d  mov        dword ptr [esi + eax*4 + 0x20], ebx
0043c771  mov        ecx, dword ptr [0x4b0cb0]
0043c777  mov        eax, dword ptr [esi + ecx*4 + 0x20]
0043c77b  mov        dx, word ptr [0x4ce568]
0043c782  mov        word ptr [eax + 2], dx
0043c786  mov        cx, word ptr [0x4b0cb0]
0043c78d  mov        dword ptr [0x4ce56c], edi
0043c793  mov        edx, dword ptr [eax + 0x9c]
0043c799  mov        word ptr [eax], cx
0043c79c  xor        edx, dword ptr [0x4cee4c]
0043c7a2  and        edx, 1
0043c7a5  xor        dword ptr [eax + 0x9c], edx
0043c7ab  pop        edi
0043c7ac  pop        esi
0043c7ad  pop        ebx
0043c7ae  ret        

// block 0043c7af
0043c7af  cmp        eax, 1
0043c7b2  jne        0x43c8b8

// block 0043c7b8
0043c7b8  mov        eax, dword ptr [0x4b0cb0]
0043c7bd  lea        eax, [eax + eax*8]
0043c7c0  mov        ecx, dword ptr [esi + eax*4 + 0xa8]
0043c7c7  mov        edx, dword ptr [esi + eax*4 + 0xb0]
0043c7ce  mov        ebx, dword ptr [esi + eax*4 + 0xb8]
0043c7d5  lea        eax, [esi + eax*4 + 0xa8]
0043c7dc  mov        dword ptr [eax + 4], ecx
0043c7df  mov        dword ptr [eax + 0xc], edx
0043c7e2  mov        dword ptr [eax + 0x14], 0xffffffff
0043c7e9  mov        ax, word ptr [ebx + 2]
0043c7ed  mov        word ptr [0x4ce568], ax
0043c7f3  mov        dword ptr [0x4ce56c], edi
0043c7f9  mov        ecx, dword ptr [ebx + 0xc]
0043c7fc  mov        dword ptr [0x4b0c44], ecx
0043c802  movsx      eax, word ptr [ebx + 0x10]
0043c806  mov        ecx, dword ptr [0x4b0cd0]
0043c80c  cmp        eax, ecx
0043c80e  mov        dword ptr [0x4b0c48], eax
0043c813  jg         0x43c81f

// block 0043c815
0043c815  mov        ecx, dword ptr [0x4b0cd4]
0043c81b  cmp        eax, ecx
0043c81d  jge        0x43c825

// block 0043c81f
0043c81f  mov        dword ptr [0x4b0c48], ecx

// block 0043c825
0043c825  mov        edx, dword ptr [ebx + 0x14]
0043c828  mov        dword ptr [0x4b0c78], edx
0043c82e  movsx      eax, word ptr [ebx + 0x18]
0043c832  mov        dword ptr [0x4b0c98], eax
0043c837  movsx      ecx, word ptr [ebx + 0x1a]
0043c83b  mov        dword ptr [0x4b0c9c], ecx
0043c841  mov        edx, dword ptr [ebx + 0x2c]
0043c844  mov        dword ptr [0x4b0ccc], edx
0043c84a  mov        eax, dword ptr [ebx + 0x38]
0043c84d  mov        dword ptr [0x4b0cc4], eax
0043c852  mov        eax, dword ptr [0x4b0cb0]
0043c857  lea        ecx, [eax + eax*8]
0043c85a  mov        edx, dword ptr [esi + ecx*4 + 0xb8]
0043c861  mov        eax, dword ptr [edx + 0x3c]
0043c864  mov        dword ptr [0x4b0cd8], eax
0043c869  movsx      ecx, word ptr [ebx + 0x1c]
0043c86d  mov        dword ptr [0x4b0ca0], ecx
0043c873  movsx      edx, word ptr [ebx + 0x1e]
0043c877  mov        dword ptr [0x4b0ca4], edx
0043c87d  mov        dword ptr [0x4b0c54], edi
0043c883  mov        dword ptr [0x4b0c50], edi
0043c889  mov        dword ptr [0x4b0c4c], edi
0043c88f  mov        dword ptr [0x4b0c58], edi
0043c895  mov        dword ptr [0x4b0c5c], edi
0043c89b  mov        edi, dword ptr [ebx + 0x20]
0043c89e  mov        esi, 0x4b0c4c
0043c8a3  call       0x422e80

// block 0043c8a8
0043c8a8  mov        edi, dword ptr [ebx + 0x24]
0043c8ab  call       0x422e80

// block 0043c8b0
0043c8b0  mov        edi, dword ptr [ebx + 0x28]
0043c8b3  call       0x422e80

// block 0043c8b8
0043c8b8  pop        edi
0043c8b9  pop        esi
0043c8ba  pop        ebx
0043c8bb  ret        
