
// block 004922c9
004922c9  mov        edi, edi
004922cb  push       ebp
004922cc  mov        ebp, esp
004922ce  mov        eax, dword ptr [ebp + 8]
004922d1  test       eax, eax
004922d3  je         0x492317

// block 004922d5
004922d5  mov        eax, dword ptr [eax]
004922d7  cmp        dword ptr [eax], 0xe06d7363
004922dd  jne        0x492317

// block 004922df
004922df  cmp        dword ptr [eax + 0x10], 3
004922e3  jne        0x492317

// block 004922e5
004922e5  mov        ecx, dword ptr [eax + 0x14]
004922e8  cmp        ecx, 0x19930520
004922ee  je         0x492300

// block 004922f0
004922f0  cmp        ecx, 0x19930521
004922f6  je         0x492300

// block 004922f8
004922f8  cmp        ecx, 0x19930522
004922fe  jne        0x492317

// block 00492300
00492300  cmp        dword ptr [eax + 0x1c], 0
00492304  jne        0x492317

// block 00492306
00492306  call       0x475467

// block 0049230b
0049230b  add        eax, 0x90
00492310  inc        dword ptr [eax]
00492312  xor        eax, eax
00492314  inc        eax
00492315  pop        ebp
00492316  ret        

// block 00492317
00492317  xor        eax, eax
00492319  pop        ebp
0049231a  ret        
