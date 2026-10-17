004b9b80 55                       push       ebp
004b9b81 8bec                     mov        ebp, esp
004b9b83 51                       push       ecx
004b9b84 894dfc                   mov        dword ptr [ebp - 4], ecx
004b9b87 8b45fc                   mov        eax, dword ptr [ebp - 4]
004b9b8a 8b08                     mov        ecx, dword ptr [eax]
004b9b8c 83e1fe                   and        ecx, 0xfffffffe
004b9b8f 8b55fc                   mov        edx, dword ptr [ebp - 4]
004b9b92 890a                     mov        dword ptr [edx], ecx
004b9b94 8b45fc                   mov        eax, dword ptr [ebp - 4]
004b9b97 8b08                     mov        ecx, dword ptr [eax]
004b9b99 83e1fd                   and        ecx, 0xfffffffd
004b9b9c 8b55fc                   mov        edx, dword ptr [ebp - 4]
004b9b9f 890a                     mov        dword ptr [edx], ecx
004b9ba1 8b45fc                   mov        eax, dword ptr [ebp - 4]
004b9ba4 8b08                     mov        ecx, dword ptr [eax]
004b9ba6 83e1fb                   and        ecx, 0xfffffffb
004b9ba9 8b55fc                   mov        edx, dword ptr [ebp - 4]
004b9bac 890a                     mov        dword ptr [edx], ecx
004b9bae 8b45fc                   mov        eax, dword ptr [ebp - 4]
004b9bb1 8b08                     mov        ecx, dword ptr [eax]
004b9bb3 83e1f7                   and        ecx, 0xfffffff7
004b9bb6 8b55fc                   mov        edx, dword ptr [ebp - 4]
004b9bb9 890a                     mov        dword ptr [edx], ecx
004b9bbb 8b45fc                   mov        eax, dword ptr [ebp - 4]
004b9bbe 8b08                     mov        ecx, dword ptr [eax]
004b9bc0 83e1ef                   and        ecx, 0xffffffef
004b9bc3 8b55fc                   mov        edx, dword ptr [ebp - 4]
004b9bc6 890a                     mov        dword ptr [edx], ecx
004b9bc8 8b45fc                   mov        eax, dword ptr [ebp - 4]
004b9bcb 8b08                     mov        ecx, dword ptr [eax]
004b9bcd 83e1df                   and        ecx, 0xffffffdf
004b9bd0 8b55fc                   mov        edx, dword ptr [ebp - 4]
004b9bd3 890a                     mov        dword ptr [edx], ecx
004b9bd5 8b45fc                   mov        eax, dword ptr [ebp - 4]
004b9bd8 8b08                     mov        ecx, dword ptr [eax]
004b9bda 83e1bf                   and        ecx, 0xffffffbf
004b9bdd 8b55fc                   mov        edx, dword ptr [ebp - 4]
004b9be0 890a                     mov        dword ptr [edx], ecx
004b9be2 8b45fc                   mov        eax, dword ptr [ebp - 4]
004b9be5 8b08                     mov        ecx, dword ptr [eax]
004b9be7 81c980000000             or         ecx, 0x80
004b9bed 8b55fc                   mov        edx, dword ptr [ebp - 4]
004b9bf0 890a                     mov        dword ptr [edx], ecx
004b9bf2 8b45fc                   mov        eax, dword ptr [ebp - 4]
004b9bf5 8b08                     mov        ecx, dword ptr [eax]
004b9bf7 81e1fffeffff             and        ecx, 0xfffffeff
004b9bfd 8b55fc                   mov        edx, dword ptr [ebp - 4]
004b9c00 890a                     mov        dword ptr [edx], ecx
004b9c02 8b45fc                   mov        eax, dword ptr [ebp - 4]
004b9c05 8be5                     mov        esp, ebp
004b9c07 5d                       pop        ebp
