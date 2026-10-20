
// block 004739a5
004739a5  push       0xc
004739a7  push       0x4aac40
004739ac  call       0x46fcec

// block 004739b1
004739b1  call       0x475467

// block 004739b6
004739b6  mov        edi, eax
004739b8  mov        eax, dword ptr [0x4ad9d4]
004739bd  test       dword ptr [edi + 0x70], eax
004739c0  je         0x4739df

// block 004739c2
004739c2  cmp        dword ptr [edi + 0x6c], 0
004739c6  je         0x4739df

// block 004739c8
004739c8  mov        esi, dword ptr [edi + 0x68]

// block 004739cb
004739cb  test       esi, esi
004739cd  jne        0x4739d7

// block 004739cf
004739cf  push       0x20
004739d1  call       0x472c0d

// block 004739d6
004739d6  pop        ecx

// block 004739d7
004739d7  mov        eax, esi
004739d9  call       0x46fd31

// block 004739de
004739de  ret        

// block 004739df
004739df  push       0xd
004739e1  call       0x46ec9a

// block 004739e6
004739e6  pop        ecx
004739e7  and        dword ptr [ebp - 4], 0
004739eb  mov        esi, dword ptr [edi + 0x68]
004739ee  mov        dword ptr [ebp - 0x1c], esi
004739f1  cmp        esi, dword ptr [0x4ad8d8]
004739f7  je         0x473a2f

// block 004739f9
004739f9  test       esi, esi
004739fb  je         0x473a17

// block 004739fd
004739fd  push       esi
004739fe  call       dword ptr [0x49815c]

// block 00473a04
00473a04  test       eax, eax
00473a06  jne        0x473a17

// block 00473a08
00473a08  cmp        esi, 0x4ad4b0
00473a0e  je         0x473a17

// block 00473a10
00473a10  push       esi
00473a11  call       0x46c941

// block 00473a16
00473a16  pop        ecx

// block 00473a17
00473a17  mov        eax, dword ptr [0x4ad8d8]
00473a1c  mov        dword ptr [edi + 0x68], eax
00473a1f  mov        esi, dword ptr [0x4ad8d8]
00473a25  mov        dword ptr [ebp - 0x1c], esi
00473a28  push       esi
00473a29  call       dword ptr [0x498160]

// block 00473a2f
00473a2f  mov        dword ptr [ebp - 4], 0xfffffffe
00473a36  call       0x473a40

// block 00473a3b
00473a3b  jmp        0x4739cb
