
// block 004da32b
004da32b  push       ebp
004da32c  mov        eax, dword ptr [0xf6ef3907]
004da331  dec        ecx
004da332  xchg       dword ptr [edi - 0x4d], ebx
004da335  int        0x18

// block 004da337
004da337  mov        byte ptr [0x8de21b68], al
004da33c  cwde       
004da33d  sbb        byte ptr [ebx], bl
004da33f  ret        0xf5ad
