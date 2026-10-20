
// block 00490ca6
00490ca6  mov        edi, edi
00490ca8  push       ebp
00490ca9  mov        ebp, esp
00490cab  cmp        dword ptr [0x4b40dc], 0
00490cb2  jne        0x490cba

// block 00490cb4
00490cb4  pop        ebp
00490cb5  jmp        0x48d94e

// block 00490cba
00490cba  push       0
00490cbc  push       dword ptr [ebp + 0x10]
00490cbf  push       dword ptr [ebp + 0xc]
00490cc2  push       dword ptr [ebp + 8]
00490cc5  call       0x490bac

// block 00490cca
00490cca  add        esp, 0x10
00490ccd  pop        ebp
00490cce  ret        
