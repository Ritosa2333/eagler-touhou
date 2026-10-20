
// block 0048ba20
0048ba20  mov        edi, edi
0048ba22  push       ebp
0048ba23  mov        ebp, esp
0048ba25  sub        esp, 0x10
0048ba28  push       dword ptr [ebp + 0xc]
0048ba2b  lea        ecx, [ebp - 0x10]
0048ba2e  call       0x46d3b4

// block 0048ba33
0048ba33  mov        eax, dword ptr [ebp - 0x10]
0048ba36  cmp        dword ptr [eax + 0xac], 1
0048ba3d  jle        0x48ba52

// block 0048ba3f
0048ba3f  lea        eax, [ebp - 0x10]
0048ba42  push       eax
0048ba43  push       4
0048ba45  push       dword ptr [ebp + 8]
0048ba48  call       0x4758eb

// block 0048ba4d
0048ba4d  add        esp, 0xc
0048ba50  jmp        0x48ba62

// block 0048ba52
0048ba52  mov        eax, dword ptr [eax + 0xc8]
0048ba58  mov        ecx, dword ptr [ebp + 8]
0048ba5b  movzx      eax, word ptr [eax + ecx*2]
0048ba5f  and        eax, 4

// block 0048ba62
0048ba62  cmp        byte ptr [ebp - 4], 0
0048ba66  je         0x48ba6f

// block 0048ba68
0048ba68  mov        ecx, dword ptr [ebp - 8]
0048ba6b  and        dword ptr [ecx + 0x70], 0xfffffffd

// block 0048ba6f
0048ba6f  leave      
0048ba70  ret        
