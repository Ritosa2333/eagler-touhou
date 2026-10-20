
// block 00496919
00496919  mov        edi, edi
0049691b  push       ebx
0049691c  mov        ebx, esp
0049691e  push       ecx
0049691f  push       ecx
00496920  and        esp, 0xfffffff0
00496923  add        esp, 4
00496926  push       ebp
00496927  mov        ebp, dword ptr [ebx + 4]
0049692a  mov        dword ptr [esp + 4], ebp
0049692e  mov        ebp, esp
00496930  sub        esp, 0x80
00496936  mov        eax, dword ptr [0x4ad138]
0049693b  xor        eax, ebp
0049693d  mov        dword ptr [ebp - 4], eax
00496940  push       dword ptr [ebx + 0x28]
00496943  lea        eax, [ebx + 0x20]
00496946  push       eax
00496947  push       dword ptr [ebx + 8]
0049694a  call       0x496491

// block 0049694f
0049694f  add        esp, 0xc
00496952  test       eax, eax
00496954  jne        0x496986

// block 00496956
00496956  mov        eax, dword ptr [ebp - 0x40]
00496959  fld        qword ptr [ebx + 0x18]
0049695c  and        eax, 0xffffffe3
0049695f  fstp       qword ptr [ebp - 0x50]
00496962  or         eax, 3
00496965  mov        dword ptr [ebp - 0x40], eax
00496968  lea        eax, [ebx + 0x20]
0049696b  push       eax
0049696c  lea        eax, [ebx + 0x10]
0049696f  push       eax
00496970  push       dword ptr [ebx + 0xc]
00496973  lea        eax, [ebx + 0x28]
00496976  push       dword ptr [ebx + 8]
00496979  push       eax
0049697a  lea        eax, [ebp - 0x80]
0049697d  push       eax
0049697e  call       0x49644b

// block 00496983
00496983  add        esp, 0x18

// block 00496986
00496986  push       dword ptr [ebx + 8]
00496989  call       0x4966c6

// block 0049698e
0049698e  add        esp, 4
00496991  cmp        dword ptr [0x4b37a0], 0
00496998  jne        0x4969c6

// block 0049699a
0049699a  test       eax, eax
0049699c  je         0x4969c6

// block 0049699e
0049699e  push       dword ptr [ebx + 0x28]
004969a1  fld        qword ptr [ebx + 0x20]
004969a4  sub        esp, 0x18
004969a7  fstp       qword ptr [esp + 0x10]
004969ab  fld        qword ptr [ebx + 0x18]
004969ae  fstp       qword ptr [esp + 8]
004969b2  fld        qword ptr [ebx + 0x10]
004969b5  fstp       qword ptr [esp]
004969b8  push       dword ptr [ebx + 0xc]
004969bb  push       eax
004969bc  call       0x4966fa

// block 004969c1
004969c1  add        esp, 0x24
004969c4  jmp        0x4969e0

// block 004969c6
004969c6  push       eax
004969c7  call       0x496673

// block 004969cc
004969cc  mov        dword ptr [esp], 0xffff
004969d3  push       dword ptr [ebx + 0x28]
004969d6  call       0x491181

// block 004969db
004969db  fld        qword ptr [ebx + 0x20]
004969de  pop        ecx
004969df  pop        ecx

// block 004969e0
004969e0  mov        ecx, dword ptr [ebp - 4]
004969e3  xor        ecx, ebp
004969e5  call       0x46c932

// block 004969ea
004969ea  mov        esp, ebp
004969ec  pop        ebp
004969ed  mov        esp, ebx
004969ef  pop        ebx
004969f0  ret        
