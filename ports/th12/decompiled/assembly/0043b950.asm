
// block 0043b950
0043b950  push       ecx
0043b951  push       esi
0043b952  xor        esi, esi
0043b954  push       edi
0043b955  cmp        dword ptr [0x4b44e8], esi
0043b95b  je         0x43bad2

// block 0043b961
0043b961  mov        eax, dword ptr [ecx + 0x1d8]
0043b967  lea        edx, [eax + eax*8]
0043b96a  cmp        dword ptr [ecx + edx*4 + 0xbc], esi
0043b971  jge        0x43b98c

// block 0043b973
0043b973  mov        dword ptr [0x4d49d0], esi
0043b979  mov        dword ptr [0x4d49dc], esi
0043b97f  mov        dword ptr [0x4d49e0], esi
0043b985  lea        eax, [esi + 1]
0043b988  pop        edi
0043b989  pop        esi
0043b98a  pop        ecx
0043b98b  ret        

// block 0043b98c
0043b98c  cmp        eax, esi
0043b98e  jl         0x43baba

// block 0043b994
0043b994  mov        eax, dword ptr [0x4d49d0]
0043b999  mov        dword ptr [0x4d49d4], eax
0043b99e  mov        eax, dword ptr [ecx + 0x1d8]
0043b9a4  lea        edx, [eax + eax*8]
0043b9a7  mov        edi, dword ptr [ecx + edx*4 + 0xbc]
0043b9ae  lea        eax, [ecx + edx*4]
0043b9b1  mov        edx, dword ptr [eax + 0xb8]
0043b9b7  cmp        edi, dword ptr [edx + 4]
0043b9ba  jge        0x43ba82

// block 0043b9c0
0043b9c0  mov        eax, dword ptr [eax + 0xac]
0043b9c6  movzx      edx, word ptr [eax]
0043b9c9  mov        edi, 0xffff
0043b9ce  cmp        dx, di
0043b9d1  jne        0x43b9ff

// block 0043b9d3
0043b9d3  cmp        word ptr [eax + 2], di
0043b9d7  jne        0x43b9ff

// block 0043b9d9
0043b9d9  cmp        word ptr [eax + 4], di
0043b9dd  jne        0x43b9ff

// block 0043b9df
0043b9df  mov        dword ptr [0x4d49d0], esi
0043b9e5  mov        dword ptr [0x4d49dc], esi
0043b9eb  mov        dword ptr [0x4d49e0], esi
0043b9f1  call       0x432850

// block 0043b9f6
0043b9f6  mov        eax, 1
0043b9fb  pop        edi
0043b9fc  pop        esi
0043b9fd  pop        ecx
0043b9fe  ret        

// block 0043b9ff
0043b9ff  movzx      eax, dx
0043ba02  mov        dword ptr [0x4d49d0], eax
0043ba07  mov        eax, dword ptr [ecx + 0x1d8]
0043ba0d  lea        edx, [eax + eax*8]
0043ba10  mov        eax, dword ptr [ecx + edx*4 + 0xac]
0043ba17  movzx      edx, word ptr [eax + 2]
0043ba1b  mov        dword ptr [0x4d49dc], edx
0043ba21  mov        eax, dword ptr [ecx + 0x1d8]
0043ba27  lea        eax, [eax + eax*8]
0043ba2a  mov        edx, dword ptr [ecx + eax*4 + 0xac]
0043ba31  movzx      eax, word ptr [edx + 4]
0043ba35  mov        dword ptr [0x4d49e0], eax
0043ba3a  mov        eax, dword ptr [ecx + 0x1d8]
0043ba40  lea        edx, [eax + eax*8 + 0x2d]
0043ba44  mov        edx, dword ptr [ecx + edx*4]
0043ba47  mov        dl, byte ptr [edx]
0043ba49  lea        eax, [eax + eax*8]
0043ba4c  lea        eax, [ecx + eax*4 + 0xac]
0043ba53  mov        byte ptr [ecx + 0x1cc], dl
0043ba59  add        dword ptr [eax], 6
0043ba5c  mov        eax, dword ptr [ecx + 0x1d0]
0043ba62  cdq        
0043ba63  mov        esi, 0x1e
0043ba68  idiv       esi
0043ba6a  test       edx, edx
0043ba6c  jne        0x43ba94

// block 0043ba6e
0043ba6e  mov        eax, dword ptr [ecx + 0x1d8]
0043ba74  add        eax, 5
0043ba77  lea        edx, [eax + eax*8]
0043ba7a  inc        dword ptr [ecx + edx*4]
0043ba7d  lea        eax, [ecx + edx*4]
0043ba80  jmp        0x43ba94

// block 0043ba82
0043ba82  mov        dword ptr [0x4d49d0], esi
0043ba88  mov        dword ptr [0x4d49dc], esi
0043ba8e  mov        dword ptr [0x4d49e0], esi

// block 0043ba94
0043ba94  mov        eax, dword ptr [ecx + 0x1d8]
0043ba9a  lea        eax, [eax + eax*8]
0043ba9d  inc        dword ptr [ecx + eax*4 + 0xbc]
0043baa4  inc        dword ptr [ecx + 0x1d0]
0043baaa  lea        eax, [ecx + eax*4 + 0xbc]
0043bab1  mov        eax, 1
0043bab6  pop        edi
0043bab7  pop        esi
0043bab8  pop        ecx
0043bab9  ret        

// block 0043baba
0043baba  mov        dword ptr [0x4d49d0], esi
0043bac0  mov        dword ptr [0x4d49dc], esi
0043bac6  mov        dword ptr [0x4d49e0], esi
0043bacc  inc        dword ptr [ecx + 0x1d0]

// block 0043bad2
0043bad2  pop        edi
0043bad3  mov        eax, 1
0043bad8  pop        esi
0043bad9  pop        ecx
0043bada  ret        
