
// block 004915c2
004915c2  mov        edi, edi
004915c4  push       ebp
004915c5  mov        ebp, esp
004915c7  push       esi
004915c8  mov        esi, ecx
004915ca  mov        dword ptr [esi], 0x49f390
004915d0  call       0x49155a

// block 004915d5
004915d5  test       byte ptr [ebp + 8], 1
004915d9  je         0x4915e2

// block 004915db
004915db  push       esi
004915dc  call       0x46ca4f

// block 004915e1
004915e1  pop        ecx

// block 004915e2
004915e2  mov        eax, esi
004915e4  pop        esi
004915e5  pop        ebp
004915e6  ret        4
