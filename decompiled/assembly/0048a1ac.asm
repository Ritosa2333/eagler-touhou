
// block 0048a1ac
0048a1ac  mov        edi, edi
0048a1ae  push       ebp
0048a1af  mov        ebp, esp
0048a1b1  sub        esp, 0x10
0048a1b4  push       esi
0048a1b5  push       dword ptr [ebp + 0xc]
0048a1b8  lea        ecx, [ebp - 0x10]
0048a1bb  call       0x46d3b4

// block 0048a1c0
0048a1c0  mov        eax, dword ptr [ebp + 8]
0048a1c3  mov        cl, byte ptr [eax]
0048a1c5  mov        esi, dword ptr [ebp - 0x10]
0048a1c8  test       cl, cl
0048a1ca  je         0x48a1e1

// block 0048a1cc
0048a1cc  mov        edx, dword ptr [esi + 0xbc]
0048a1d2  mov        edx, dword ptr [edx]
0048a1d4  mov        dl, byte ptr [edx]

// block 0048a1d6
0048a1d6  cmp        cl, dl
0048a1d8  je         0x48a1e1

// block 0048a1da
0048a1da  inc        eax
0048a1db  mov        cl, byte ptr [eax]
0048a1dd  test       cl, cl
0048a1df  jne        0x48a1d6

// block 0048a1e1
0048a1e1  mov        cl, byte ptr [eax]
0048a1e3  inc        eax
0048a1e4  test       cl, cl
0048a1e6  je         0x48a21e

// block 0048a1e8
0048a1e8  jmp        0x48a1f5

// block 0048a1ea
0048a1ea  cmp        cl, 0x65
0048a1ed  je         0x48a1fb

// block 0048a1ef
0048a1ef  cmp        cl, 0x45
0048a1f2  je         0x48a1fb

// block 0048a1f4
0048a1f4  inc        eax

// block 0048a1f5
0048a1f5  mov        cl, byte ptr [eax]
0048a1f7  test       cl, cl
0048a1f9  jne        0x48a1ea

// block 0048a1fb
0048a1fb  mov        edx, eax

// block 0048a1fd
0048a1fd  dec        eax
0048a1fe  cmp        byte ptr [eax], 0x30
0048a201  je         0x48a1fd

// block 0048a203
0048a203  mov        ecx, dword ptr [esi + 0xbc]
0048a209  mov        ecx, dword ptr [ecx]
0048a20b  push       ebx
0048a20c  mov        bl, byte ptr [eax]
0048a20e  cmp        bl, byte ptr [ecx]
0048a210  pop        ebx
0048a211  jne        0x48a214

// block 0048a213
0048a213  dec        eax

// block 0048a214
0048a214  mov        cl, byte ptr [edx]
0048a216  inc        eax
0048a217  inc        edx
0048a218  mov        byte ptr [eax], cl
0048a21a  test       cl, cl
0048a21c  jne        0x48a214

// block 0048a21e
0048a21e  cmp        byte ptr [ebp - 4], 0
0048a222  pop        esi
0048a223  je         0x48a22c

// block 0048a225
0048a225  mov        eax, dword ptr [ebp - 8]
0048a228  and        dword ptr [eax + 0x70], 0xfffffffd

// block 0048a22c
0048a22c  leave      
0048a22d  ret        
