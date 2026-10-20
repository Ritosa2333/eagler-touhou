
// block 0046db4a
0046db4a  mov        edi, edi
0046db4c  push       ebp
0046db4d  mov        ebp, esp
0046db4f  push       ecx
0046db50  push       ebx
0046db51  push       edi
0046db52  mov        edi, dword ptr [ebp + 8]
0046db55  xor        ebx, ebx
0046db57  mov        dword ptr [ebp - 4], ebx
0046db5a  cmp        edi, ebx
0046db5c  jne        0x46db7e

// block 0046db5e
0046db5e  call       0x46e96f

// block 0046db63
0046db63  push       ebx
0046db64  push       ebx
0046db65  push       ebx
0046db66  push       ebx
0046db67  push       ebx
0046db68  mov        dword ptr [eax], 0x16
0046db6e  call       0x470f2d

// block 0046db73
0046db73  add        esp, 0x14
0046db76  or         eax, 0xffffffff
0046db79  jmp        0x46dbff

// block 0046db7e
0046db7e  push       esi
0046db7f  call       0x475279

// block 0046db84
0046db84  push       0x214
0046db89  push       1
0046db8b  call       0x477813

// block 0046db90
0046db90  mov        esi, eax
0046db92  pop        ecx
0046db93  pop        ecx
0046db94  cmp        esi, ebx
0046db96  je         0x46dbe2

// block 0046db98
0046db98  call       0x475467

// block 0046db9d
0046db9d  push       dword ptr [eax + 0x6c]
0046dba0  push       esi
0046dba1  call       0x475307

// block 0046dba6
0046dba6  mov        eax, dword ptr [ebp + 0x10]
0046dba9  pop        ecx
0046dbaa  pop        ecx
0046dbab  push       esi
0046dbac  push       4
0046dbae  push       esi
0046dbaf  push       0x46dad3
0046dbb4  push       dword ptr [ebp + 0xc]
0046dbb7  mov        dword ptr [esi + 0x54], edi
0046dbba  push       ebx
0046dbbb  mov        dword ptr [esi + 0x58], eax
0046dbbe  call       dword ptr [0x4980f0]

// block 0046dbc4
0046dbc4  mov        edi, eax
0046dbc6  mov        dword ptr [esi + 4], edi
0046dbc9  cmp        edi, ebx
0046dbcb  je         0x46dbd9

// block 0046dbcd
0046dbcd  push       edi
0046dbce  call       dword ptr [0x498108]

// block 0046dbd4
0046dbd4  cmp        eax, -1
0046dbd7  jne        0x46dbfc

// block 0046dbd9
0046dbd9  call       dword ptr [0x4980e4]

// block 0046dbdf
0046dbdf  mov        dword ptr [ebp - 4], eax

// block 0046dbe2
0046dbe2  push       esi
0046dbe3  call       0x46c941

// block 0046dbe8
0046dbe8  pop        ecx
0046dbe9  cmp        dword ptr [ebp - 4], ebx
0046dbec  je         0x46dbf7

// block 0046dbee
0046dbee  push       dword ptr [ebp - 4]
0046dbf1  call       0x46e995

// block 0046dbf6
0046dbf6  pop        ecx

// block 0046dbf7
0046dbf7  or         eax, 0xffffffff
0046dbfa  jmp        0x46dbfe

// block 0046dbfc
0046dbfc  mov        eax, edi

// block 0046dbfe
0046dbfe  pop        esi

// block 0046dbff
0046dbff  pop        edi
0046dc00  pop        ebx
0046dc01  leave      
0046dc02  ret        
