
// block 00436100
00436100  push       ecx
00436101  fldz       
00436103  push       esi
00436104  mov        esi, dword ptr [0x4b4514]
0043610a  mov        dword ptr [esi + 0xa28], 1
00436114  mov        eax, dword ptr [esi + 0xc430]
0043611a  push       edi
0043611b  xor        edi, edi
0043611d  mov        edx, 0xfff0bdc1
00436122  mov        ecx, 0x4b2ed0
00436127  test       al, 1
00436129  jne        0x43614c

// block 0043612b
0043612b  or         eax, 1
0043612e  fst        dword ptr [esi + 0xc428]
00436134  mov        dword ptr [esi + 0xc424], edi
0043613a  mov        dword ptr [esi + 0xc420], edx
00436140  mov        dword ptr [esi + 0xc42c], ecx
00436146  mov        dword ptr [esi + 0xc430], eax

// block 0043614c
0043614c  fld        dword ptr [0x4a3da8]
00436152  mov        dword ptr [esi + 0xc424], 0xffffffff
0043615c  fstp       dword ptr [esi + 0xc428]
00436162  mov        dword ptr [esi + 0xc420], 0xfffffffe
0043616c  mov        eax, dword ptr [esi + 0xa40]
00436172  test       al, 1
00436174  jne        0x436197

// block 00436176
00436176  or         eax, 1
00436179  fst        dword ptr [esi + 0xa38]
0043617f  mov        dword ptr [esi + 0xa34], edi
00436185  mov        dword ptr [esi + 0xa30], edx
0043618b  mov        dword ptr [esi + 0xa3c], ecx
00436191  mov        dword ptr [esi + 0xa40], eax

// block 00436197
00436197  fst        dword ptr [esi + 0xa38]
0043619d  mov        dword ptr [esi + 0xa34], edi
004361a3  mov        dword ptr [esi + 0xa30], 0xffffffff
004361ad  mov        eax, dword ptr [esi + 0xa54]
004361b3  test       al, 1
004361b5  jne        0x4361d8

// block 004361b7
004361b7  or         eax, 1
004361ba  fst        dword ptr [esi + 0xa4c]
004361c0  mov        dword ptr [esi + 0xa48], edi
004361c6  mov        dword ptr [esi + 0xa44], edx
004361cc  mov        dword ptr [esi + 0xa50], ecx
004361d2  mov        dword ptr [esi + 0xa54], eax

// block 004361d8
004361d8  fstp       dword ptr [esi + 0xa4c]
004361de  mov        dword ptr [esi + 0xa48], edi
004361e4  mov        dword ptr [esi + 0xa44], 0xffffffff
004361ee  and        dword ptr [esi + 0xc414], 0xfffffff8
004361f5  mov        dword ptr [esi + 0xa20], edi
004361fb  mov        dword ptr [esi + 0xa24], edi
00436201  mov        eax, dword ptr [esi + 0x8258]
00436207  push       eax
00436208  call       0x461a70

// block 0043620d
0043620d  mov        eax, dword ptr [0x4b43e4]
00436212  mov        dword ptr [esi + 0x8258], edi
00436218  mov        ecx, dword ptr [0x4b0c9c]
0043621e  mov        edx, dword ptr [0x4b0c98]
00436224  push       ecx
00436225  push       edx
00436226  push       eax
00436227  call       0x41ce60

// block 0043622c
0043622c  push       esi
0043622d  call       0x435900

// block 00436232
00436232  push       esi
00436233  call       0x4385b0

// block 00436238
00436238  fld1       
0043623a  pop        edi
0043623b  fstp       dword ptr [esi + 0x825c]
00436241  pop        esi
00436242  pop        ecx
00436243  ret        
