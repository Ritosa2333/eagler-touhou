
// block 004db30a
004db30a  jecxz      0x4db28c

// block 004db30c
004db30c  mov        ah, 7
004db30e  pop        eax
004db30f  and        ecx, dword ptr [ebx]
004db311  dec        ecx
004db312  sub        byte ptr [edx - 0x6ddb0c7b], dh
004db318  jno        0x4db2e4

// block 004db31a
004db31a  cmp        esp, edi
004db31c  scasd      eax, dword ptr es:[edi]
004db31d  jl         0x4db319

// block 004db31f
004db31f  push       es
004db320  sbb        eax, 0x7d5455ed
004db325  loopne     0x4db371

// block 004db327
004db327  cwde       
004db328  pop        si
004db32b  dec        eax

// block 004db343
004db343  movsb      byte ptr es:[edi], byte ptr [esi]

// block 004db348
004db348  pop        esp

// block 004db34a

// block 004db371
004db371  scasb      al, byte ptr es:[edi]
004db372  cmp        esp, dword ptr [ebx]
004db374  jecxz      0x4db34a

// block 004db376
004db376  fcomi      st(5)
004db378  jge        0x4db343
