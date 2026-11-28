import 'package:flutter_test/flutter_test.dart';
import 'package:touhouonline/network_diagnostics.dart';

void main() {
  test('VPN and documentation addresses do not count as a public route', () {
    expect(
      readDirectNetwork([
        'candidate:1 1 udp 2113937151 198.18.0.1 12345 typ host',
        'candidate:2 1 udp 2113937151 192.168.1.4 12345 typ host',
        'candidate:3 1 udp 101 2001:db8::2 33421 typ srflx raddr :: rport 12345',
        'invalid',
      ]),
      (nat: null, ipv6: false),
    );
  });

  test('NAT mapping compares candidates from the same source', () {
    const first =
        'candidate:4 1 udp 101 1.2.3.4 51354 typ srflx raddr 192.168.1.4 rport 12345';
    const differentSource =
        'candidate:5 1 udp 101 1.2.3.4 51454 typ srflx raddr 192.168.1.4 rport 12346';
    const differentMapping =
        'candidate:6 1 udp 101 1.2.3.4 51554 typ srflx raddr 192.168.1.4 rport 12345';
    expect(readDirectNetwork([first, differentSource]), (
      nat: 'NAT3',
      ipv6: false,
    ));
    expect(readDirectNetwork([first, differentMapping]), (
      nat: 'NAT4',
      ipv6: false,
    ));
    expect(
      readDirectNetwork([
        'candidate:7 1 udp 2113937151 1.2.3.4 12345 typ host',
        'candidate:8 1 udp 101 2606:4700::1 33421 typ srflx raddr :: rport 12345',
      ]),
      (nat: 'NAT1', ipv6: true),
    );
  });
}
