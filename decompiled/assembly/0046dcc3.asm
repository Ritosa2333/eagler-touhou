
// block 0046dcc3
0046dcc3  mov        edi, edi
0046dcc5  push       ebp
0046dcc6  mov        ebp, esp
0046dcc8  sub        esp, 0x20
0046dccb  push       edi
0046dccc  push       esi
0046dccd  call       0x475860

// block 0046dcd2
0046dcd2  xor        edi, edi
0046dcd4  pop        ecx
0046dcd5  cmp        esi, edi
0046dcd7  jne        0x46dcf6

// block 0046dcd9
0046dcd9  call       0x46e96f

// block 0046dcde
0046dcde  push       edi
0046dcdf  push       edi
0046dce0  push       edi
0046dce1  push       edi
0046dce2  push       edi
0046dce3  mov        dword ptr [eax], 0x16
0046dce9  call       0x470f2d

// block 0046dcee
0046dcee  add        esp, 0x14
0046dcf1  or         eax, 0xffffffff
0046dcf4  jmp        0x46dd2a

// block 0046dcf6
0046dcf6  cmp        dword ptr [ebp + 0xc], edi
0046dcf9  je         0x46dcd9

// block 0046dcfb
0046dcfb  mov        ecx, 0x7fffffff
0046dd00  mov        dword ptr [ebp - 0x14], 0x49
0046dd07  mov        dword ptr [ebp - 0x18], esi
0046dd0a  mov        dword ptr [ebp - 0x20], esi
0046dd0d  mov        dword ptr [ebp - 0x1c], ecx
0046dd10  cmp        eax, ecx
0046dd12  ja         0x46dd17

// block 0046dd14
0046dd14  mov        dword ptr [ebp - 0x1c], eax

// block 0046dd17
0046dd17  push       dword ptr [ebp + 0x14]
0046dd1a  lea        eax, [ebp - 0x20]
0046dd1d  push       dword ptr [ebp + 0x10]
0046dd20  push       dword ptr [ebp + 0xc]
0046dd23  push       eax
0046dd24  call       dword ptr [ebp + 8]

// block 0046dd27
0046dd27  add        esp, 0x10

// block 0046dd2a
0046dd2a  pop        edi
0046dd2b  leave      
0046dd2c  ret        
