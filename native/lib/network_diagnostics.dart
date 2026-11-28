import 'dart:async';
import 'dart:convert';
import 'dart:io';

import 'package:flutter_webrtc/flutter_webrtc.dart';
import 'package:uuid/uuid.dart';

enum NetworkCheck { websocket, turn, nat, ipv6 }

enum NetworkCheckState { pending, good, warning, failed, unavailable }

typedef NetworkReading = ({
  NetworkCheckState state,
  int? milliseconds,
  String? nat,
});

const networkPending = (
  state: NetworkCheckState.pending,
  milliseconds: null,
  nat: null,
);
const networkFailed = (
  state: NetworkCheckState.failed,
  milliseconds: null,
  nat: null,
);
const networkUnavailable = (
  state: NetworkCheckState.unavailable,
  milliseconds: null,
  nat: null,
);

class NetworkDiagnostics {
  NetworkDiagnostics({required this.endpoint, required this.origin});

  final Uri? endpoint;
  final Uri origin;
  final _http = HttpClient()..connectionTimeout = const Duration(seconds: 3);
  final _cancelled = Completer<void>();
  final _peers = <RTCPeerConnection>{};
  WebSocket? _socket;
  bool _closed = false;

  Future<void> run(void Function(NetworkCheck, NetworkReading) update) async {
    try {
      await Future.wait([_service(update), _direct(update)]);
    } finally {
      close();
    }
  }

  void close() {
    if (_closed) return;
    _closed = true;
    _cancelled.complete();
    _http.close(force: true);
    _socket?.close().ignore();
    for (final peer in _peers) {
      peer.dispose().ignore();
    }
    _peers.clear();
  }

  Future<T> _until<T>(Future<T> operation, Duration timeout) => Future.any<T>([
    operation,
    _cancelled.future.then<T>((_) => throw StateError('Diagnostic closed')),
  ]).timeout(timeout);

  Future<void> _service(
    void Function(NetworkCheck, NetworkReading) update,
  ) async {
    final relay = endpoint;
    if (relay == null) {
      if (!_closed) {
        update(NetworkCheck.websocket, networkUnavailable);
        update(NetworkCheck.turn, networkUnavailable);
      }
      return;
    }
    List<Map<String, Object>> servers;
    try {
      final probe = await _websocket(relay);
      if (_closed) return;
      update(NetworkCheck.websocket, _latency(probe.milliseconds));
      servers = probe.servers;
    } catch (_) {
      if (!_closed) {
        update(NetworkCheck.websocket, networkFailed);
        update(NetworkCheck.turn, networkUnavailable);
      }
      return;
    }
    if (servers.isEmpty) {
      update(NetworkCheck.turn, networkUnavailable);
      return;
    }
    try {
      final delay = await _until(_turn(servers), const Duration(seconds: 36));
      if (!_closed) update(NetworkCheck.turn, _latency(delay));
    } catch (_) {
      if (!_closed) update(NetworkCheck.turn, networkFailed);
    }
  }

  Future<({int milliseconds, List<Map<String, Object>> servers})> _websocket(
    Uri relay,
  ) async {
    final query = {...relay.queryParameters}
      ..removeWhere(
        (key, _) => const [
          'room',
          'run',
          'lobby',
          'player',
          'players',
          'signal',
          'spectator',
          'member',
          'directory',
        ].contains(key),
      )
      ..['diagnostic'] = '1';
    final address = relay.replace(queryParameters: query, fragment: '');
    final opening = WebSocket.connect(
      address.toString(),
      headers: {'Origin': origin.origin},
      customClient: _http,
    );
    final WebSocket socket;
    try {
      socket = await _until(opening, const Duration(seconds: 3));
    } catch (_) {
      opening.then((socket) => socket.close()).ignore();
      rethrow;
    }
    if (_closed) {
      await socket.close();
      throw StateError('Diagnostic closed');
    }
    _socket = socket;
    final packets = StreamIterator(socket);
    try {
      final first = await _packet(packets, 'diagnostic-ready');
      final samples = <int>[];
      for (var index = 0; index < 3; index++) {
        final nonce = const Uuid().v4();
        final timer = Stopwatch()..start();
        socket.add(jsonEncode({'type': 'diagnostic-ping', 'nonce': nonce}));
        await _packet(packets, 'diagnostic-pong', nonce);
        samples.add(timer.elapsedMicroseconds);
      }
      return (
        milliseconds: _median(samples, 1000),
        servers: _iceServers(first['iceServers']),
      );
    } finally {
      await packets.cancel();
      _socket = null;
      socket.close().ignore();
    }
  }

  Future<Map<String, dynamic>> _packet(
    StreamIterator<dynamic> packets,
    String type, [
    String? nonce,
  ]) async {
    final timer = Stopwatch()..start();
    for (var index = 0; index < 8; index++) {
      final remaining = const Duration(seconds: 2) - timer.elapsed;
      if (remaining <= Duration.zero ||
          !await _until(packets.moveNext(), remaining)) {
        break;
      }
      final value = packets.current;
      if (value is! String || value.length > 65536) continue;
      final packet = jsonDecode(value);
      if (packet is Map<String, dynamic> &&
          packet['type'] == type &&
          (nonce == null || packet['nonce'] == nonce)) {
        return packet;
      }
    }
    throw StateError('Missing diagnostic response');
  }

  Future<void> _direct(
    void Function(NetworkCheck, NetworkReading) update,
  ) async {
    const servers = [
      {'urls': 'stun:stun.cloudflare.com:3478'},
      {'urls': 'stun:stun.l.google.com:19302'},
    ];
    final candidates = <String>[];
    RTCPeerConnection? peer;
    try {
      final active = await _until(
        _peer({'iceServers': servers}),
        const Duration(seconds: 3),
      );
      peer = active;
      final gathered = Completer<void>();
      active.onIceCandidate = (event) {
        if (event.candidate case final String candidate) {
          candidates.add(candidate);
        }
      };
      active.onIceGatheringState = (state) {
        if (state == RTCIceGatheringState.RTCIceGatheringStateComplete &&
            !gathered.isCompleted) {
          gathered.complete();
        }
      };
      await _until(
        active.createDataChannel('network-check', RTCDataChannelInit()),
        const Duration(seconds: 2),
      );
      final offer = await _until(
        active.createOffer(),
        const Duration(seconds: 2),
      );
      await _until(
        active.setLocalDescription(offer),
        const Duration(seconds: 2),
      );
      try {
        await _until(gathered.future, const Duration(seconds: 8));
      } on TimeoutException {
        // Candidates gathered so far are still valid evidence.
      }
      if (_closed) return;
      final (nat: nat, ipv6: ipv6) = readDirectNetwork(candidates);
      update(
        NetworkCheck.nat,
        nat == null
            ? networkFailed
            : (
                state: nat == 'NAT4'
                    ? NetworkCheckState.warning
                    : NetworkCheckState.good,
                milliseconds: null,
                nat: nat,
              ),
      );
      update(
        NetworkCheck.ipv6,
        ipv6
            ? (state: NetworkCheckState.good, milliseconds: null, nat: null)
            : networkUnavailable,
      );
    } catch (_) {
      if (!_closed) {
        update(NetworkCheck.nat, networkFailed);
        update(NetworkCheck.ipv6, networkUnavailable);
      }
    } finally {
      if (peer != null && _peers.remove(peer)) {
        await peer.dispose().catchError((Object _) {});
      }
    }
  }

  Future<RTCPeerConnection> _peer(Map<String, dynamic> settings) async {
    final peer = await createPeerConnection(settings);
    if (_closed) {
      await peer.dispose();
      throw StateError('Diagnostic closed');
    }
    _peers.add(peer);
    return peer;
  }

  Future<int> _turn(List<Map<String, Object>> servers) async {
    final settings = {'iceServers': servers, 'iceTransportPolicy': 'relay'};
    final left = await _peer(settings);
    final right = await _peer(settings);
    final toRight = _IceForwarder(right);
    final toLeft = _IceForwarder(left);
    left.onIceCandidate = toRight.add;
    right.onIceCandidate = toLeft.add;
    right.onDataChannel = (channel) => channel.onMessage = (message) {
      if (!_closed) channel.send(message).ignore();
    };
    final channel = await left.createDataChannel(
      'network-check',
      RTCDataChannelInit(),
    );
    final opened = Completer<void>();
    channel.onDataChannelState = (state) {
      if (state == RTCDataChannelState.RTCDataChannelOpen &&
          !opened.isCompleted) {
        opened.complete();
      }
    };
    final offer = await left.createOffer();
    await left.setLocalDescription(offer);
    await right.setRemoteDescription(offer);
    toRight.flush();
    final answer = await right.createAnswer();
    await right.setLocalDescription(answer);
    await left.setRemoteDescription(answer);
    toLeft.flush();
    if (channel.state != RTCDataChannelState.RTCDataChannelOpen) {
      await _until(opened.future, const Duration(seconds: 35));
    }
    if (!await _usesTurn(left)) throw StateError('TURN pair was not selected');
    final samples = <int>[];
    for (var index = 0; index < 3; index++) {
      final nonce = const Uuid().v4();
      final timer = Stopwatch()..start();
      final reply = channel.messageStream
          .firstWhere((message) => !message.isBinary && message.text == nonce)
          .timeout(const Duration(milliseconds: 2500));
      await _until(
        Future.wait<void>([
          channel.send(RTCDataChannelMessage(nonce)),
          reply.then<void>((_) {}),
        ]),
        const Duration(milliseconds: 2600),
      );
      samples.add(timer.elapsedMicroseconds);
    }
    return _median(samples, 2000);
  }
}

NetworkReading _latency(int value) => (
  state: value < 180 ? NetworkCheckState.good : NetworkCheckState.warning,
  milliseconds: value,
  nat: null,
);

int _median(List<int> values, int scale) {
  values.sort();
  return (values[values.length ~/ 2] / scale).round().clamp(1, 60000);
}

bool _isTurn(String url) =>
    const ['turn', 'turns'].contains(Uri.tryParse(url)?.scheme);

List<Map<String, Object>> _iceServers(Object? value) {
  if (value is! List) return [];
  final servers = <Map<String, Object>>[];
  for (final server in value.take(16).whereType<Map>()) {
    final urls = switch (server['urls']) {
      final String url => [url],
      final List urls => urls.take(12).whereType<String>(),
      _ => const <String>[],
    };
    final turn = urls
        .where((url) => url.length <= 300 && _isTurn(url))
        .toList();
    if (turn.isEmpty) continue;
    servers.add({
      'urls': turn,
      if (server['username'] case final String user when user.length <= 256)
        'username': user,
      if (server['credential'] case final String credential
          when credential.length <= 256)
        'credential': credential,
    });
  }
  return servers;
}

class _IceForwarder {
  _IceForwarder(this.target);

  final RTCPeerConnection target;
  final _pending = <RTCIceCandidate>[];
  bool _ready = false;

  void add(RTCIceCandidate candidate) {
    if (candidate.candidate?.isNotEmpty != true) return;
    if (_ready) {
      target.addCandidate(candidate).ignore();
    } else {
      _pending.add(candidate);
    }
  }

  void flush() {
    _ready = true;
    for (final candidate in _pending) {
      add(candidate);
    }
    _pending.clear();
  }
}

Future<bool> _usesTurn(RTCPeerConnection peer) async {
  final rows = await peer.getStats();
  final values = {for (final row in rows) row.id: row.values};
  final selected = rows
      .where((row) => row.type == 'transport')
      .map((row) => values[row.values['selectedCandidatePairId']])
      .nonNulls
      .firstOrNull;
  final pair =
      selected ??
      rows
          .where(
            (row) =>
                row.type == 'candidate-pair' &&
                row.values['state'] == 'succeeded' &&
                row.values['nominated'] == true,
          )
          .firstOrNull
          ?.values;
  return values[pair?['localCandidateId']]?['candidateType'] == 'relay' &&
      values[pair?['remoteCandidateId']]?['candidateType'] == 'relay';
}

class _Candidate {
  const _Candidate(this.type, this.address, this.port, this.related);

  final String type;
  final InternetAddress address;
  final int port;
  final String? related;

  static _Candidate? parse(String? raw) {
    if (raw == null) return null;
    final parts = raw.trim().split(RegExp(r'\s+'));
    if (parts.length < 8 || parts[6] != 'typ') return null;
    final address = InternetAddress.tryParse(parts[4]);
    final port = int.tryParse(parts[5]);
    if (address == null || port == null) return null;
    final extra = {
      for (var i = 8; i + 1 < parts.length; i += 2) parts[i]: parts[i + 1],
    };
    final related = extra['raddr'];
    final relatedPort = int.tryParse(extra['rport'] ?? '');
    return _Candidate(
      parts[7],
      address,
      port,
      related != null && relatedPort != null && relatedPort > 0
          ? '$related:$relatedPort'
          : null,
    );
  }
}

bool _globalIpv4(InternetAddress address) {
  final parts = address.rawAddress;
  if (parts.length != 4) return false;
  final p = parts[0];
  final q = parts[1];
  final r = parts[2];
  return p != 0 &&
      p != 10 &&
      p != 127 &&
      p < 224 &&
      !(p == 100 && q >= 64 && q <= 127) &&
      !(p == 169 && q == 254) &&
      !(p == 172 && q >= 16 && q <= 31) &&
      !(p == 192 && (q == 168 || q == 0 && (r == 0 || r == 2))) &&
      !(p == 198 && (q == 18 || q == 19 || q == 51 && r == 100)) &&
      !(p == 203 && q == 0 && r == 113);
}

({String? nat, bool ipv6}) readDirectNetwork(Iterable<String> candidateLines) {
  final candidates = candidateLines.map(_Candidate.parse).nonNulls.toList();
  return (
    nat: _classifyNat(candidates),
    ipv6: candidates.any((candidate) {
      final bytes = candidate.address.rawAddress;
      return candidate.type == 'srflx' &&
          bytes.length == 16 &&
          (bytes.first & 0xe0) == 0x20 &&
          !(bytes[0] == 0x20 &&
              bytes[1] == 1 &&
              bytes[2] == 0x0d &&
              bytes[3] == 0xb8);
    }),
  );
}

String? _classifyNat(List<_Candidate> candidates) {
  if (candidates.any(
    (candidate) => candidate.type == 'host' && _globalIpv4(candidate.address),
  )) {
    return 'NAT1';
  }
  final reflexive = candidates.where(
    (candidate) => candidate.type == 'srflx' && _globalIpv4(candidate.address),
  );
  final mappings = <String, Set<String>>{};
  for (final candidate in reflexive) {
    if (candidate.related case final String related) {
      (mappings[related] ??= {}).add(
        '${candidate.address.address}:${candidate.port}',
      );
    }
  }
  if (mappings.values.any((mapped) => mapped.length > 1)) return 'NAT4';
  return reflexive.isEmpty ? null : 'NAT3';
}
