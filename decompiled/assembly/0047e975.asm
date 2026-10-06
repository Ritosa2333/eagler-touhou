
// block 0047e975
0047e975  mov        edi, edi
0047e977  push       ebp
0047e978  mov        ebp, esp
0047e97a  mov        eax, dword ptr [0x4b4318]
0047e97f  mov        al, byte ptr [eax]
0047e981  mov        ecx, dword ptr [ebp + 8]
0047e984  test       al, al
0047e986  je         0x47e9a2

// block 0047e988
0047e988  cmp        al, 0x41
0047e98a  je         0x47e990

// block 0047e98c
0047e98c  push       2
0047e98e  jmp        0x47e9a4

// block 0047e990
0047e990  inc        dword ptr [0x4b4318]
0047e996  push       0x49de00
0047e99b  call       0x47e59a

// block 0047e9a0
0047e9a0  jmp        0x47e9a9

// block 0047e9a2
0047e9a2  push       1

// block 0047e9a4
0047e9a4  call       0x47e1ce

// block 0047e9a9
0047e9a9  mov        eax, dword ptr [ebp + 8]
0047e9ac  pop        ebp
0047e9ad  ret        
