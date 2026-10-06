
// block 004072d0
004072d0  mov        edx, dword ptr [0x4ce8cc]
004072d6  push       ebx
004072d7  push       ebp
004072d8  mov        ebp, dword ptr [esp + 0xc]
004072dc  mov        eax, dword ptr [ebp + 0x40]
004072df  push       esi
004072e0  push       edi
004072e1  lea        edi, [ebp + 0x40]
004072e4  push       eax
004072e5  call       0x461920

// block 004072ea
004072ea  mov        esi, eax
004072ec  test       esi, esi
004072ee  jne        0x4072f2

// block 004072f0
004072f0  mov        dword ptr [edi], eax

// block 004072f2
004072f2  push       0x28
004072f4  call       0x406f60

// block 004072f9
004072f9  test       esi, esi
004072fb  jne        0x407313

// block 004072fd
004072fd  mov        ecx, dword ptr [ebp + 0x48]
00407300  push       ecx
00407301  lea        ebx, [esi + 1]
00407304  call       0x461970

// block 00407309
00407309  pop        edi
0040730a  pop        esi
0040730b  pop        ebp
0040730c  or         eax, 0xffffffff
0040730f  pop        ebx
00407310  ret        4

// block 00407313
00407313  cmp        dword ptr [ebp + 0x18], 0x12c
0040731a  jle        0x40734a

// block 0040731c
0040731c  mov        edx, dword ptr [edi]
0040731e  push       edx
0040731f  mov        ebx, 1
00407324  call       0x461970

// block 00407329
00407329  mov        eax, dword ptr [ebp + 0x48]
0040732c  push       eax
0040732d  call       0x461970

// block 00407332
00407332  fld1       
00407334  mov        ecx, dword ptr [0x4b4514]
0040733a  fstp       dword ptr [ecx + 0x825c]
00407340  pop        edi
00407341  pop        esi
00407342  pop        ebp
00407343  or         eax, 0xffffffff
00407346  pop        ebx
00407347  ret        4

// block 0040734a
0040734a  mov        eax, dword ptr [0x4b4514]
0040734f  fld        dword ptr [0x4a43a8]
00407355  mov        edx, dword ptr [eax + 0x97c]
0040735b  fstp       dword ptr [eax + 0x825c]
00407361  lea        esi, [ebp + 0x508]
00407367  mov        dword ptr [esi], edx
00407369  mov        ecx, dword ptr [eax + 0x980]
0040736f  mov        dword ptr [esi + 4], ecx
00407372  mov        edx, dword ptr [eax + 0x984]
00407378  mov        eax, edi
0040737a  mov        dword ptr [esi + 8], edx
0040737d  call       0x461e30

// block 00407382
00407382  mov        ebx, ebp
00407384  call       0x407580

// block 00407389
00407389  pop        edi
0040738a  pop        esi
0040738b  pop        ebp
0040738c  xor        eax, eax
0040738e  pop        ebx
0040738f  ret        4
