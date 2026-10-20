
// block 0048219f
0048219f  mov        edi, edi
004821a1  push       ebp
004821a2  mov        ebp, esp
004821a4  push       0x26
004821a6  push       dword ptr [ebp + 0x10]
004821a9  push       dword ptr [ebp + 0xc]
004821ac  push       dword ptr [ebp + 8]
004821af  call       0x48206c

// block 004821b4
004821b4  mov        eax, dword ptr [ebp + 8]
004821b7  add        esp, 0x10
004821ba  pop        ebp
004821bb  ret        
