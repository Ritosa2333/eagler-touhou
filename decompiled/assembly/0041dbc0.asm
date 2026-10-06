
// block 0041dbc0
0041dbc0  push       ebx
0041dbc1  mov        ebx, dword ptr [0x4b43e4]
0041dbc7  push       ebp
0041dbc8  push       esi
0041dbc9  mov        esi, dword ptr [ebx + 0x6d30]
0041dbcf  xor        ebp, ebp
0041dbd1  cmp        esi, ebp
0041dbd3  je         0x41dbe9

// block 0041dbd5
0041dbd5  call       0x41cd90

// block 0041dbda
0041dbda  push       esi
0041dbdb  call       0x46ca4f

// block 0041dbe0
0041dbe0  add        esp, 4
0041dbe3  mov        dword ptr [ebx + 0x6d30], ebp

// block 0041dbe9
0041dbe9  test       byte ptr [0x4b0ce0], 9
0041dbf0  jne        0x41dc3d

// block 0041dbf2
0041dbf2  mov        esi, dword ptr [0x4ce8cc]
0041dbf8  add        esi, 0x4b512c
0041dbfe  push       edi
0041dbff  mov        edi, dword ptr [esi]
0041dc01  cmp        edi, ebp
0041dc03  je         0x41dc17

// block 0041dc05
0041dc05  call       0x4604e0

// block 0041dc0a
0041dc0a  mov        eax, dword ptr [esi]
0041dc0c  push       eax
0041dc0d  call       0x46ca4f

// block 0041dc12
0041dc12  add        esp, 4
0041dc15  mov        dword ptr [esi], ebp

// block 0041dc17
0041dc17  mov        eax, dword ptr [ebx + 0x6d34]
0041dc1d  mov        dword ptr [ebx + 0x6ce4], ebp
0041dc23  pop        edi
0041dc24  cmp        eax, ebp
0041dc26  je         0x41dc37

// block 0041dc28
0041dc28  push       eax
0041dc29  call       0x46c941

// block 0041dc2e
0041dc2e  add        esp, 4
0041dc31  mov        dword ptr [ebx + 0x6d34], ebp

// block 0041dc37
0041dc37  mov        dword ptr [ebx + 0x6d34], ebp

// block 0041dc3d
0041dc3d  pop        esi
0041dc3e  pop        ebp
0041dc3f  pop        ebx
0041dc40  ret        
