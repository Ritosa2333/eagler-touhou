004d88c0  55                                 push       ebp
004d88c1  8b ec                              mov        ebp, esp
004d88c3  51                                 push       ecx
004d88c4  89 4d fc                           mov        dword ptr [ebp - 4], ecx
004d88c7  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004d88ca  8b 08                              mov        ecx, dword ptr [eax]
004d88cc  83 e1 fe                           and        ecx, 0xfffffffe
004d88cf  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004d88d2  89 0a                              mov        dword ptr [edx], ecx
004d88d4  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004d88d7  8b 08                              mov        ecx, dword ptr [eax]
004d88d9  83 e1 fd                           and        ecx, 0xfffffffd
004d88dc  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004d88df  89 0a                              mov        dword ptr [edx], ecx
004d88e1  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004d88e4  8b 08                              mov        ecx, dword ptr [eax]
004d88e6  83 e1 fb                           and        ecx, 0xfffffffb
004d88e9  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004d88ec  89 0a                              mov        dword ptr [edx], ecx
004d88ee  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004d88f1  8b 08                              mov        ecx, dword ptr [eax]
004d88f3  83 e1 f7                           and        ecx, 0xfffffff7
004d88f6  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004d88f9  89 0a                              mov        dword ptr [edx], ecx
004d88fb  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004d88fe  8b 08                              mov        ecx, dword ptr [eax]
004d8900  83 e1 ef                           and        ecx, 0xffffffef
004d8903  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004d8906  89 0a                              mov        dword ptr [edx], ecx
004d8908  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004d890b  8b 08                              mov        ecx, dword ptr [eax]
004d890d  83 e1 9f                           and        ecx, 0xffffff9f
004d8910  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004d8913  89 0a                              mov        dword ptr [edx], ecx
004d8915  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004d8918  8b 08                              mov        ecx, dword ptr [eax]
004d891a  81 c9 80 00 00 00                  or         ecx, 0x80
004d8920  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004d8923  89 0a                              mov        dword ptr [edx], ecx
004d8925  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004d8928  8b 08                              mov        ecx, dword ptr [eax]
004d892a  81 e1 ff fe ff ff                  and        ecx, 0xfffffeff
004d8930  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004d8933  89 0a                              mov        dword ptr [edx], ecx
004d8935  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004d8938  8b 08                              mov        ecx, dword ptr [eax]
004d893a  81 e1 ff fd ff ff                  and        ecx, 0xfffffdff
004d8940  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004d8943  89 0a                              mov        dword ptr [edx], ecx
004d8945  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004d8948  8b 08                              mov        ecx, dword ptr [eax]
004d894a  81 e1 ff fb ff ff                  and        ecx, 0xfffffbff
004d8950  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004d8953  89 0a                              mov        dword ptr [edx], ecx
004d8955  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004d8958  8b 08                              mov        ecx, dword ptr [eax]
004d895a  81 c9 00 08 00 00                  or         ecx, 0x800
004d8960  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004d8963  89 0a                              mov        dword ptr [edx], ecx
004d8965  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004d8968  8b 08                              mov        ecx, dword ptr [eax]
004d896a  81 e1 ff ef ff ff                  and        ecx, 0xffffefff
004d8970  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004d8973  89 0a                              mov        dword ptr [edx], ecx
004d8975  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004d8978  8b 08                              mov        ecx, dword ptr [eax]
004d897a  81 e1 ff df ff ff                  and        ecx, 0xffffdfff
004d8980  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004d8983  89 0a                              mov        dword ptr [edx], ecx
004d8985  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004d8988  8b e5                              mov        esp, ebp
004d898a  5d                                 pop        ebp
004d898b  c3                                 ret        
