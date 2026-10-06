
// block 004097f0
004097f0  push       -1
004097f2  push       0x49770b
004097f7  mov        eax, dword ptr fs:[0]
004097fd  push       eax
004097fe  push       ecx
004097ff  push       esi
00409800  push       edi
00409801  mov        eax, dword ptr [0x4ad138]
00409806  xor        eax, esp
00409808  push       eax
00409809  lea        eax, [esp + 0x10]
0040980d  mov        dword ptr fs:[0], eax
00409813  push       0x4debe0
00409818  call       0x46c9ea

// block 0040981d
0040981d  mov        esi, eax
0040981f  add        esp, 4
00409822  mov        dword ptr [esp + 0xc], esi
00409826  mov        dword ptr [esp + 0x18], 0
0040982e  test       esi, esi
00409830  je         0x409867

// block 00409832
00409832  push       0x4094f0
00409837  push       0x4094b0
0040983c  push       0x7d1
00409841  push       0x9f8
00409846  lea        eax, [esi + 0x64]
00409849  push       eax
0040984a  call       0x46ce49

// block 0040984f
0040984f  push       0x4debe0
00409854  push       0
00409856  push       esi
00409857  call       0x477420

// block 0040985c
0040985c  add        esp, 0xc
0040985f  mov        dword ptr [0x4b43c8], esi
00409865  jmp        0x409869

// block 00409867
00409867  xor        esi, esi

// block 00409869
00409869  mov        edi, esi
0040986b  mov        dword ptr [esp + 0x18], 0xffffffff
00409873  call       0x409530

// block 00409878
00409878  test       eax, eax
0040987a  je         0x4098a3

// block 0040987c
0040987c  test       esi, esi
0040987e  je         0x40988f

// block 00409880
00409880  push       esi
00409881  call       0x4096d0

// block 00409886
00409886  push       esi
00409887  call       0x46ca4f

// block 0040988c
0040988c  add        esp, 4

// block 0040988f
0040988f  xor        eax, eax
00409891  mov        ecx, dword ptr [esp + 0x10]
00409895  mov        dword ptr fs:[0], ecx
0040989c  pop        ecx
0040989d  pop        edi
0040989e  pop        esi
0040989f  add        esp, 0x10
004098a2  ret        

// block 004098a3
004098a3  mov        eax, esi
004098a5  mov        ecx, dword ptr [esp + 0x10]
004098a9  mov        dword ptr fs:[0], ecx
004098b0  pop        ecx
004098b1  pop        edi
004098b2  pop        esi
004098b3  add        esp, 0x10
004098b6  ret        
