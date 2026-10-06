
// block 00481f3b
00481f3b  mov        edi, edi
00481f3d  push       ebp
00481f3e  mov        ebp, esp
00481f40  sub        esp, 0x18
00481f43  mov        eax, dword ptr [ebp - 4]
00481f46  mov        edx, 0xffff0000
00481f4b  and        dword ptr [ebp - 4], edx
00481f4e  xor        ecx, ecx
00481f50  and        dword ptr [ebp - 8], ecx
00481f53  and        eax, edx
00481f55  mov        edx, dword ptr [0x4b431c]
00481f5b  test       edx, edx
00481f5d  je         0x481fc2

// block 00481f5f
00481f5f  cmp        byte ptr [edx], 0x3f
00481f62  jne        0x481fb3

// block 00481f64
00481f64  mov        al, byte ptr [edx + 1]
00481f67  cmp        al, 0x40
00481f69  jne        0x481f8f

// block 00481f6b
00481f6b  add        dword ptr [0x4b4318], 2
00481f72  lea        eax, [ebp - 0x10]
00481f75  push       eax
00481f76  call       0x481298

// block 00481f7b
00481f7b  push       eax
00481f7c  lea        eax, [ebp - 0x18]
00481f7f  push       0x49e08c
00481f84  push       eax
00481f85  call       0x47ec4a

// block 00481f8a
00481f8a  add        esp, 0x10
00481f8d  jmp        0x481fbd

// block 00481f8f
00481f8f  cmp        al, 0x24
00481f91  jne        0x481fb3

// block 00481f93
00481f93  lea        eax, [ebp - 0x18]
00481f96  push       0
00481f98  push       eax
00481f99  call       0x4800f3

// block 00481f9e
00481f9e  pop        ecx
00481f9f  pop        ecx
00481fa0  mov        ecx, dword ptr [eax]
00481fa2  mov        eax, dword ptr [eax + 4]
00481fa5  cmp        al, 2
00481fa7  jne        0x481fc2

// block 00481fa9
00481fa9  mov        eax, dword ptr [0x4b431c]
00481fae  mov        dword ptr [0x4b4318], eax

// block 00481fb3
00481fb3  lea        eax, [ebp - 0x18]
00481fb6  push       eax
00481fb7  call       0x481298

// block 00481fbc
00481fbc  pop        ecx

// block 00481fbd
00481fbd  mov        ecx, dword ptr [eax]
00481fbf  mov        eax, dword ptr [eax + 4]

// block 00481fc2
00481fc2  cmp        al, 3
00481fc4  jne        0x481fca

// block 00481fc6
00481fc6  xor        eax, eax
00481fc8  leave      
00481fc9  ret        

// block 00481fca
00481fca  cmp        al, 2
00481fcc  je         0x481fed

// block 00481fce
00481fce  test       dword ptr [0x4b4328], 0x1000
00481fd8  jne        0x481fe5

// block 00481fda
00481fda  mov        edx, dword ptr [0x4b4318]
00481fe0  cmp        byte ptr [edx], 0
00481fe3  jne        0x481fed

// block 00481fe5
00481fe5  mov        dword ptr [ebp - 8], ecx
00481fe8  mov        dword ptr [ebp - 4], eax
00481feb  jmp        0x481ffb

// block 00481fed
00481fed  push       dword ptr [0x4b431c]
00481ff3  lea        ecx, [ebp - 8]
00481ff6  call       0x47e896

// block 00481ffb
00481ffb  mov        eax, dword ptr [0x4b4320]
00482000  test       eax, eax
00482002  jne        0x48202c

// block 00482004
00482004  mov        ecx, dword ptr [ebp - 8]
00482007  test       ecx, ecx
00482009  je         0x48200f

// block 0048200b
0048200b  mov        eax, dword ptr [ecx]
0048200d  call       dword ptr [eax]

// block 0048200f
0048200f  inc        eax
00482010  mov        dword ptr [0x4b4324], eax
00482015  add        eax, 7
00482018  and        eax, 0xfffffff8
0048201b  push       eax
0048201c  call       dword ptr [0x4b42f8]

// block 00482022
00482022  pop        ecx
00482023  mov        dword ptr [0x4b4320], eax
00482028  test       eax, eax
0048202a  je         0x48206a

// block 0048202c
0048202c  push       dword ptr [0x4b4324]
00482032  lea        ecx, [ebp - 8]
00482035  push       eax
00482036  call       0x47e213

// block 0048203b
0048203b  mov        eax, dword ptr [0x4b4320]
00482040  mov        edx, eax
00482042  jmp        0x48205b

// block 00482044
00482044  cmp        cl, 0x20
00482047  jne        0x482057

// block 00482049
00482049  inc        eax
0048204a  mov        byte ptr [edx], cl
0048204c  inc        edx
0048204d  jmp        0x482050

// block 0048204f
0048204f  inc        eax

// block 00482050
00482050  cmp        byte ptr [eax], 0x20
00482053  je         0x48204f

// block 00482055
00482055  jmp        0x48205b

// block 00482057
00482057  mov        byte ptr [edx], cl
00482059  inc        edx
0048205a  inc        eax

// block 0048205b
0048205b  mov        cl, byte ptr [eax]
0048205d  test       cl, cl
0048205f  jne        0x482044

// block 00482061
00482061  mov        al, cl
00482063  mov        byte ptr [edx], al
00482065  mov        eax, dword ptr [0x4b4320]

// block 0048206a
0048206a  leave      
0048206b  ret        
