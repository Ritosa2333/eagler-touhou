
// block 0047e8cb
0047e8cb  mov        edi, edi
0047e8cd  push       ebp
0047e8ce  mov        ebp, esp
0047e8d0  push       ecx
0047e8d1  push       ecx
0047e8d2  mov        eax, dword ptr [0x4b4318]
0047e8d7  mov        al, byte ptr [eax]
0047e8d9  test       al, al
0047e8db  je         0x47e966

// block 0047e8e1
0047e8e1  movsx      eax, al
0047e8e4  sub        eax, 0x41
0047e8e7  inc        dword ptr [0x4b4318]
0047e8ed  cmp        eax, 0xc
0047e8f0  ja         0x47e962

// block 0047e8f2
0047e8f2  mov        ecx, dword ptr [0x4b4328]
0047e8f8  and        dword ptr [ebp - 8], 0
0047e8fc  and        dword ptr [ebp - 4], 0xffff0000
0047e903  shr        ecx, 1
0047e905  not        ecx
0047e907  test       cl, 1
0047e90a  je         0x47e952

// block 0047e90c
0047e90c  and        eax, 0xfffffffe
0047e90f  sub        eax, 0
0047e912  je         0x47e93f

// block 0047e914
0047e914  push       2
0047e916  pop        ecx
0047e917  sub        eax, ecx
0047e919  je         0x47e93c

// block 0047e91b
0047e91b  sub        eax, ecx
0047e91d  je         0x47e938

// block 0047e91f
0047e91f  sub        eax, ecx
0047e921  je         0x47e934

// block 0047e923
0047e923  sub        eax, ecx
0047e925  je         0x47e930

// block 0047e927
0047e927  sub        eax, 4
0047e92a  jne        0x47e952

// block 0047e92c
0047e92c  push       6
0047e92e  jmp        0x47e941

// block 0047e930
0047e930  push       5
0047e932  jmp        0x47e941

// block 0047e934
0047e934  push       3
0047e936  jmp        0x47e941

// block 0047e938
0047e938  push       4
0047e93a  jmp        0x47e941

// block 0047e93c
0047e93c  push       ecx
0047e93d  jmp        0x47e941

// block 0047e93f
0047e93f  push       1

// block 0047e941
0047e941  call       0x47dc0b

// block 0047e946
0047e946  add        esp, 4
0047e949  push       eax
0047e94a  lea        ecx, [ebp - 8]
0047e94d  call       0x47e896

// block 0047e952
0047e952  mov        eax, dword ptr [ebp + 8]
0047e955  mov        ecx, dword ptr [ebp - 8]
0047e958  mov        dword ptr [eax], ecx
0047e95a  mov        ecx, dword ptr [ebp - 4]
0047e95d  mov        dword ptr [eax + 4], ecx
0047e960  leave      
0047e961  ret        

// block 0047e962
0047e962  push       2
0047e964  jmp        0x47e968

// block 0047e966
0047e966  push       1

// block 0047e968
0047e968  mov        ecx, dword ptr [ebp + 8]
0047e96b  call       0x47e1ce

// block 0047e970
0047e970  mov        eax, dword ptr [ebp + 8]
0047e973  leave      
0047e974  ret        
