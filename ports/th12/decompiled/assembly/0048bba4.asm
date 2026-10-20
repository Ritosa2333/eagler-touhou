
// block 0048bba4
0048bba4  mov        edi, edi
0048bba6  push       ebp
0048bba7  mov        ebp, esp
0048bba9  sub        esp, 0x10
0048bbac  push       dword ptr [ebp + 0xc]
0048bbaf  lea        ecx, [ebp - 0x10]
0048bbb2  call       0x46d3b4

// block 0048bbb7
0048bbb7  mov        eax, dword ptr [ebp - 0x10]
0048bbba  cmp        dword ptr [eax + 0xac], 1
0048bbc1  jle        0x48bbd6

// block 0048bbc3
0048bbc3  lea        eax, [ebp - 0x10]
0048bbc6  push       eax
0048bbc7  push       0x10
0048bbc9  push       dword ptr [ebp + 8]
0048bbcc  call       0x4758eb

// block 0048bbd1
0048bbd1  add        esp, 0xc
0048bbd4  jmp        0x48bbe6

// block 0048bbd6
0048bbd6  mov        eax, dword ptr [eax + 0xc8]
0048bbdc  mov        ecx, dword ptr [ebp + 8]
0048bbdf  movzx      eax, word ptr [eax + ecx*2]
0048bbe3  and        eax, 0x10

// block 0048bbe6
0048bbe6  cmp        byte ptr [ebp - 4], 0
0048bbea  je         0x48bbf3

// block 0048bbec
0048bbec  mov        ecx, dword ptr [ebp - 8]
0048bbef  and        dword ptr [ecx + 0x70], 0xfffffffd

// block 0048bbf3
0048bbf3  leave      
0048bbf4  ret        
