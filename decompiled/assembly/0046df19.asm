
// block 0046df19
0046df19  mov        edi, edi
0046df1b  push       ebp
0046df1c  mov        ebp, esp
0046df1e  push       esi
0046df1f  push       dword ptr [0x4b41dc]
0046df25  call       0x4751de

// block 0046df2a
0046df2a  push       dword ptr [ebp + 8]
0046df2d  mov        esi, eax
0046df2f  call       0x475163

// block 0046df34
0046df34  pop        ecx
0046df35  pop        ecx
0046df36  mov        dword ptr [0x4b41dc], eax
0046df3b  mov        eax, esi
0046df3d  pop        esi
0046df3e  pop        ebp
0046df3f  ret        
