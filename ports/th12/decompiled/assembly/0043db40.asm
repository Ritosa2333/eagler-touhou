
// block 0043db40
0043db40  push       ebx
0043db41  push       ebp
0043db42  push       esi
0043db43  push       edi
0043db44  mov        edi, eax
0043db46  mov        eax, dword ptr [0x4b43b8]
0043db4b  mov        ecx, dword ptr [eax + 0x18fb4]
0043db51  push       0x24
0043db53  mov        dword ptr [edi + 0x10], ecx
0043db56  call       0x46c9ea

// block 0043db5b
0043db5b  xor        ebp, ebp
0043db5d  add        esp, 4
0043db60  cmp        eax, ebp
0043db62  je         0x43db80

// block 0043db64
0043db64  and        dword ptr [eax + 4], 0xfffffffe
0043db68  mov        dword ptr [eax + 8], ebp
0043db6b  mov        dword ptr [eax + 0xc], ebp
0043db6e  mov        dword ptr [eax + 0x10], ebp
0043db71  mov        dword ptr [eax], ebp
0043db73  mov        dword ptr [eax + 0x14], eax
0043db76  mov        dword ptr [eax + 0x18], ebp
0043db79  mov        dword ptr [eax + 0x1c], ebp
0043db7c  mov        esi, eax
0043db7e  jmp        0x43db82

// block 0043db80
0043db80  xor        esi, esi

// block 0043db82
0043db82  mov        edx, dword ptr [esi + 4]
0043db85  and        edx, 0xfffffffd
0043db88  or         edx, 1
0043db8b  mov        ebx, 0xf
0043db90  mov        dword ptr [esi + 8], 0x43e230
0043db97  mov        dword ptr [esi + 0xc], ebp
0043db9a  mov        dword ptr [esi + 0x10], ebp
0043db9d  mov        dword ptr [esi + 0x20], edi
0043dba0  mov        dword ptr [esi + 4], edx
0043dba3  call       0x462380

// block 0043dba8
0043dba8  push       0x24
0043dbaa  mov        dword ptr [edi + 8], esi
0043dbad  call       0x46c9ea

// block 0043dbb2
0043dbb2  add        esp, 4
0043dbb5  cmp        eax, ebp
0043dbb7  je         0x43dbd5

// block 0043dbb9
0043dbb9  and        dword ptr [eax + 4], 0xfffffffe
0043dbbd  mov        dword ptr [eax + 8], ebp
0043dbc0  mov        dword ptr [eax + 0xc], ebp
0043dbc3  mov        dword ptr [eax + 0x10], ebp
0043dbc6  mov        dword ptr [eax], ebp
0043dbc8  mov        dword ptr [eax + 0x14], eax
0043dbcb  mov        dword ptr [eax + 0x18], ebp
0043dbce  mov        dword ptr [eax + 0x1c], ebp
0043dbd1  mov        esi, eax
0043dbd3  jmp        0x43dbd7

// block 0043dbd5
0043dbd5  xor        esi, esi

// block 0043dbd7
0043dbd7  mov        eax, dword ptr [esi + 4]
0043dbda  and        eax, 0xfffffffd
0043dbdd  or         eax, 1
0043dbe0  mov        ebx, 0x2b
0043dbe5  mov        dword ptr [esi + 8], 0x43e240
0043dbec  mov        dword ptr [esi + 0xc], ebp
0043dbef  mov        dword ptr [esi + 0x10], ebp
0043dbf2  mov        dword ptr [esi + 0x20], edi
0043dbf5  mov        dword ptr [esi + 4], eax
0043dbf8  call       0x462420

// block 0043dbfd
0043dbfd  mov        dword ptr [edi + 0xc], esi
0043dc00  lea        esi, [edi + 0x18]
0043dc03  mov        edi, dword ptr [edi + 0x10]
0043dc06  call       0x402520

// block 0043dc0b
0043dc0b  mov        ecx, 0xc4
0043dc10  mov        eax, esi
0043dc12  mov        edx, edi
0043dc14  mov        dword ptr [esi + 0x3f8], edi
0043dc1a  call       0x454b80

// block 0043dc1f
0043dc1f  pop        edi
0043dc20  pop        esi
0043dc21  pop        ebp
0043dc22  xor        eax, eax
0043dc24  pop        ebx
0043dc25  ret        
