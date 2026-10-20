
// block 0048bc79
0048bc79  mov        edi, edi
0048bc7b  push       ebp
0048bc7c  mov        ebp, esp
0048bc7e  cmp        dword ptr [0x4b40dc], 0
0048bc85  jne        0x48bc9b

// block 0048bc87
0048bc87  mov        eax, dword ptr [ebp + 8]
0048bc8a  mov        ecx, dword ptr [0x4adaa8]
0048bc90  movzx      eax, word ptr [ecx + eax*2]
0048bc94  and        eax, 0x107
0048bc99  pop        ebp
0048bc9a  ret        

// block 0048bc9b
0048bc9b  push       0
0048bc9d  push       dword ptr [ebp + 8]
0048bca0  call       0x48bc23

// block 0048bca5
0048bca5  pop        ecx
0048bca6  pop        ecx
0048bca7  pop        ebp
0048bca8  ret        
