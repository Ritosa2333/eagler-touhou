
// block 0048d3b5
0048d3b5  mov        edi, edi
0048d3b7  push       ebp
0048d3b8  mov        ebp, esp
0048d3ba  cmp        dword ptr [ebp + 8], 0
0048d3be  je         0x48d3e7

// block 0048d3c0
0048d3c0  push       esi
0048d3c1  mov        esi, dword ptr [ebp + 0xc]
0048d3c4  test       dword ptr [esi + 0xc], 0x1000
0048d3cb  je         0x48d3e6

// block 0048d3cd
0048d3cd  push       esi
0048d3ce  call       0x48d12a

// block 0048d3d3
0048d3d3  and        dword ptr [esi + 0xc], 0xffffeeff
0048d3da  and        dword ptr [esi + 0x18], 0
0048d3de  and        dword ptr [esi], 0
0048d3e1  and        dword ptr [esi + 8], 0
0048d3e5  pop        ecx

// block 0048d3e6
0048d3e6  pop        esi

// block 0048d3e7
0048d3e7  pop        ebp
0048d3e8  ret        
