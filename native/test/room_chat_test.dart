import 'package:flutter_test/flutter_test.dart';
import 'package:touhouonline/room_chat.dart';

void main() {
  test('room text history, moderation, and delivery use Relay responses', () {
    final outgoing = <Map<String, Object>>[];
    final chat = RoomChat((message) {
      outgoing.add(message);
      return true;
    });
    addTearDown(chat.dispose);
    const room = 'th06mp-4157';
    void relay(Map<String, dynamic> value) => chat.receive(
      value,
      room: room,
      serial: 12,
      participantName: (_, _) => null,
    );
    Map<String, Object> message(int id, String author) => {
      'type': 'free-chat',
      'room': room,
      'serial': 12,
      'clientId': 'fixture-$id',
      'name': '成员',
      'text': '来看第 $id 条',
      'seat': 1,
      'authorId': author,
      'messageId': id,
    };

    expect(chat.sendText('游客', 12), isFalse);
    relay({'type': 'chat-status', 'registered': true, 'authorId': 'myid'});
    expect(chat.sendText('  保留这一条  ', 12), isTrue);
    expect(chat.sendText('并发草稿', 12), isFalse);
    expect(outgoing.single, {
      'type': 'free-chat',
      'text': '保留这一条',
      'serial': 12,
    });
    relay({'type': 'chat-result', 'action': 'free-chat', 'ok': true});
    expect(chat.busy, isFalse);
    expect(chat.resultRevision, 1);
    expect(chat.result, isNull);

    relay(message(5, 'second'));
    relay({
      'type': 'chat-history',
      'room': room,
      'throughId': 4,
      'entries': [message(3, 'myid'), message(4, 'first')],
    });
    relay(message(5, 'second'));
    expect(chat.messages.map((row) => row.id), [3, 4, 5]);
    expect(chat.report(chat.messages.first, ''), isFalse);
    expect(chat.report(chat.messages.last, ' 测试样例 '), isTrue);
    expect(outgoing.last, {
      'type': 'chat-report',
      'messageId': 5,
      'reason': '测试样例',
    });
    relay({'type': 'chat-result', 'action': 'chat-report', 'ok': true});
    expect(chat.result, RoomChatResult.reportSent);
    chat.mute('fixture-4');
    relay({'type': 'chat-retract', 'authorId': 'second', 'throughId': 5});
    relay(message(5, 'second'));
    expect(chat.messages.map((row) => row.id), [3]);
    relay({
      'type': 'chat-retract',
      'authorId': 'myid',
      'throughId': 3,
      'bannedUntil': -1,
    });
    expect(chat.messages, isEmpty);
    expect(chat.sendText('禁言时', 12), isFalse);
    chat.connectionLost();
    expect(chat.registered, isFalse);
  });
}
