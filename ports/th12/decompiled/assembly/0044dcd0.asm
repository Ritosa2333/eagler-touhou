
// block 0044dcd0
0044dcd0  mov        eax, dword ptr [edx + 0x110]
0044dcd6  imul       eax, dword ptr [edx + 0x108]
0044dcdd  push       esi
0044dcde  mov        esi, dword ptr [edx + 0x120]
0044dce4  mov        edx, dword ptr [edx + 0x100]
0044dcea  sub        edx, 0x15
0044dced  push       edi
0044dcee  je         0x44dd4e

// block 0044dcf0
0044dcf0  sub        edx, 4
0044dcf3  je         0x44dd27

// block 0044dcf5
0044dcf5  sub        edx, 1
0044dcf8  je         0x44dcff

// block 0044dcfa
0044dcfa  pop        edi
0044dcfb  xor        al, al
0044dcfd  pop        esi
0044dcfe  ret        

// block 0044dcff
0044dcff  mov        edi, 1
0044dd04  and        cl, 0xf0
0044dd07  cmp        eax, edi
0044dd09  jle        0x44dd61

// block 0044dd0b
0044dd0b  jmp        0x44dd10

// block 0044dd10
0044dd10  mov        dl, byte ptr [edi + esi]
0044dd13  and        dl, 0xf
0044dd16  or         dl, cl
0044dd18  mov        byte ptr [edi + esi], dl
0044dd1b  add        edi, 2
0044dd1e  cmp        edi, eax
0044dd20  jl         0x44dd10

// block 0044dd22
0044dd22  pop        edi
0044dd23  mov        al, 1
0044dd25  pop        esi
0044dd26  ret        

// block 0044dd27
0044dd27  neg        cl
0044dd29  sbb        cl, cl
0044dd2b  mov        edi, 1
0044dd30  and        cl, 0x80
0044dd33  cmp        eax, edi
0044dd35  jle        0x44dd61

// block 0044dd37
0044dd37  mov        dl, byte ptr [edi + esi]
0044dd3a  and        dl, 0x7f
0044dd3d  or         dl, cl
0044dd3f  mov        byte ptr [edi + esi], dl
0044dd42  add        edi, 2
0044dd45  cmp        edi, eax
0044dd47  jl         0x44dd37

// block 0044dd49
0044dd49  pop        edi
0044dd4a  mov        al, 1
0044dd4c  pop        esi
0044dd4d  ret        

// block 0044dd4e
0044dd4e  mov        edx, 2
0044dd53  cmp        eax, edx
0044dd55  jle        0x44dd61

// block 0044dd57
0044dd57  mov        byte ptr [edx + esi], cl
0044dd5a  add        edx, 4
0044dd5d  cmp        edx, eax
0044dd5f  jl         0x44dd57

// block 0044dd61
0044dd61  pop        edi
0044dd62  mov        al, 1
0044dd64  pop        esi
0044dd65  ret        
