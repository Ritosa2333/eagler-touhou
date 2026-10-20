
// block 00474181
00474181  push       0xc
00474183  push       0x4aac80
00474188  call       0x46fcec

// block 0047418d
0047418d  call       0x475467

// block 00474192
00474192  mov        esi, eax
00474194  mov        eax, dword ptr [0x4ad9d4]
00474199  test       dword ptr [esi + 0x70], eax
0047419c  je         0x4741c0

// block 0047419e
0047419e  cmp        dword ptr [esi + 0x6c], 0
004741a2  je         0x4741c0

// block 004741a4
004741a4  call       0x475467

// block 004741a9
004741a9  mov        esi, dword ptr [eax + 0x6c]

// block 004741ac
004741ac  test       esi, esi
004741ae  jne        0x4741b8

// block 004741b0
004741b0  push       0x20
004741b2  call       0x472c0d

// block 004741b7
004741b7  pop        ecx

// block 004741b8
004741b8  mov        eax, esi
004741ba  call       0x46fd31

// block 004741bf
004741bf  ret        

// block 004741c0
004741c0  push       0xc
004741c2  call       0x46ec9a

// block 004741c7
004741c7  pop        ecx
004741c8  and        dword ptr [ebp - 4], 0
004741cc  lea        eax, [esi + 0x6c]
004741cf  mov        edi, dword ptr [0x4adab8]
004741d5  call       0x474143

// block 004741da
004741da  mov        dword ptr [ebp - 0x1c], eax
004741dd  mov        dword ptr [ebp - 4], 0xfffffffe
004741e4  call       0x4741eb

// block 004741e9
004741e9  jmp        0x4741ac
