
// block 0048d524
0048d524  mov        edi, edi
0048d526  push       ebp
0048d527  mov        ebp, esp
0048d529  mov        eax, 0xffff
0048d52e  sub        esp, 0x14
0048d531  cmp        word ptr [ebp + 8], ax
0048d535  jne        0x48d53d

// block 0048d537
0048d537  and        dword ptr [ebp - 4], 0
0048d53b  jmp        0x48d5a2

// block 0048d53d
0048d53d  mov        eax, 0x100
0048d542  cmp        word ptr [ebp + 8], ax
0048d546  jae        0x48d562

// block 0048d548
0048d548  movzx      eax, word ptr [ebp + 8]
0048d54c  mov        ecx, dword ptr [0x4adea4]
0048d552  mov        ax, word ptr [ecx + eax*2]
0048d556  and        ax, word ptr [ebp + 0xc]
0048d55a  movzx      eax, ax
0048d55d  mov        dword ptr [ebp - 4], eax
0048d560  jmp        0x48d5a2

// block 0048d562
0048d562  push       dword ptr [ebp + 0x10]
0048d565  lea        ecx, [ebp - 0x14]
0048d568  call       0x46d3b4

// block 0048d56d
0048d56d  mov        eax, dword ptr [ebp - 0x14]
0048d570  push       dword ptr [eax + 0x14]
0048d573  push       dword ptr [eax + 4]
0048d576  lea        eax, [ebp - 4]
0048d579  push       eax
0048d57a  push       1
0048d57c  lea        eax, [ebp + 8]
0048d57f  push       eax
0048d580  lea        eax, [ebp - 0x14]
0048d583  push       1
0048d585  push       eax
0048d586  call       0x48ed72

// block 0048d58b
0048d58b  add        esp, 0x1c
0048d58e  test       eax, eax
0048d590  jne        0x48d595

// block 0048d592
0048d592  and        dword ptr [ebp - 4], eax

// block 0048d595
0048d595  cmp        byte ptr [ebp - 8], 0
0048d599  je         0x48d5a2

// block 0048d59b
0048d59b  mov        eax, dword ptr [ebp - 0xc]
0048d59e  and        dword ptr [eax + 0x70], 0xfffffffd

// block 0048d5a2
0048d5a2  movzx      eax, word ptr [ebp - 4]
0048d5a6  movzx      ecx, word ptr [ebp + 0xc]
0048d5aa  and        eax, ecx
0048d5ac  leave      
0048d5ad  ret        
