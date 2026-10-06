
// block 00472414
00472414  mov        edi, edi
00472416  push       ebp
00472417  mov        ebp, esp
00472419  sub        esp, 0x20
0047241c  push       ebx
0047241d  xor        ebx, ebx
0047241f  cmp        dword ptr [ebp + 0x14], ebx
00472422  jne        0x472444

// block 00472424
00472424  call       0x46e96f

// block 00472429
00472429  push       ebx
0047242a  push       ebx
0047242b  push       ebx
0047242c  push       ebx
0047242d  push       ebx
0047242e  mov        dword ptr [eax], 0x16
00472434  call       0x470f2d

// block 00472439
00472439  add        esp, 0x14
0047243c  or         eax, 0xffffffff
0047243f  jmp        0x4724dd

// block 00472444
00472444  push       esi
00472445  mov        esi, dword ptr [ebp + 0xc]
00472448  push       edi
00472449  mov        edi, dword ptr [ebp + 0x10]
0047244c  cmp        edi, ebx
0047244e  je         0x472471

// block 00472450
00472450  cmp        esi, ebx
00472452  jne        0x472471

// block 00472454
00472454  call       0x46e96f

// block 00472459
00472459  push       ebx
0047245a  push       ebx
0047245b  push       ebx
0047245c  push       ebx
0047245d  push       ebx
0047245e  mov        dword ptr [eax], 0x16
00472464  call       0x470f2d

// block 00472469
00472469  add        esp, 0x14
0047246c  or         eax, 0xffffffff
0047246f  jmp        0x4724db

// block 00472471
00472471  mov        eax, 0x7fffffff
00472476  mov        dword ptr [ebp - 0x1c], eax
00472479  cmp        edi, eax
0047247b  ja         0x472480

// block 0047247d
0047247d  mov        dword ptr [ebp - 0x1c], edi

// block 00472480
00472480  push       dword ptr [ebp + 0x1c]
00472483  lea        eax, [ebp - 0x20]
00472486  push       dword ptr [ebp + 0x18]
00472489  mov        dword ptr [ebp - 0x14], 0x42
00472490  push       dword ptr [ebp + 0x14]
00472493  mov        dword ptr [ebp - 0x18], esi
00472496  push       eax
00472497  mov        dword ptr [ebp - 0x20], esi
0047249a  call       dword ptr [ebp + 8]

// block 0047249d
0047249d  add        esp, 0x10
004724a0  mov        dword ptr [ebp + 0x14], eax
004724a3  cmp        esi, ebx
004724a5  je         0x4724db

// block 004724a7
004724a7  cmp        eax, ebx
004724a9  jl         0x4724cd

// block 004724ab
004724ab  dec        dword ptr [ebp - 0x1c]
004724ae  js         0x4724b7

// block 004724b0
004724b0  mov        eax, dword ptr [ebp - 0x20]
004724b3  mov        byte ptr [eax], bl
004724b5  jmp        0x4724c8

// block 004724b7
004724b7  lea        eax, [ebp - 0x20]
004724ba  push       eax
004724bb  push       ebx
004724bc  call       0x46ffdb

// block 004724c1
004724c1  pop        ecx
004724c2  pop        ecx
004724c3  cmp        eax, -1
004724c6  je         0x4724cd

// block 004724c8
004724c8  mov        eax, dword ptr [ebp + 0x14]
004724cb  jmp        0x4724db

// block 004724cd
004724cd  xor        eax, eax
004724cf  cmp        dword ptr [ebp - 0x1c], ebx
004724d2  mov        byte ptr [esi + edi - 1], bl
004724d6  setge      al
004724d9  dec        eax
004724da  dec        eax

// block 004724db
004724db  pop        edi
004724dc  pop        esi

// block 004724dd
004724dd  pop        ebx
004724de  leave      
004724df  ret        
