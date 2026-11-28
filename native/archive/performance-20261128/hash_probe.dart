import 'dart:async';
import 'dart:convert';
import 'dart:isolate';
import 'dart:io';
import 'dart:typed_data';
import 'package:crypto/crypto.dart';

Future<void> main() async {
  final bytes = Uint8List.fromList(List.generate(8*1024*1024,(i)=>(i*17+31)&255));
  final output = <String,dynamic>{'mode':'aot','input':'8 MiB, current cloud total byte limit, deterministic bytes','iterations':6};
  for(final worker in [false,true]) {
    final gap=Stopwatch()..start();
    var last=0,maxGap=0,ticks=0;
    final timer=Timer.periodic(const Duration(milliseconds:1),(_){final now=gap.elapsedMicroseconds;final delta=now-last;if(delta>maxGap)maxGap=delta;last=now;ticks++;});
    await Future<void>.delayed(const Duration(milliseconds:20));
    final elapsed=Stopwatch()..start();
    String? value;
    for(var i=0;i<6;i++) { value=worker?await Isolate.run(()=>sha256.convert(bytes).toString()):sha256.convert(bytes).toString(); }
    elapsed.stop();
    await Future<void>.delayed(const Duration(milliseconds:20));
    timer.cancel();
    output[worker?'worker':'inline']={'totalMs':elapsed.elapsedMicroseconds/1000,'maxEventLoopGapMs':maxGap/1000,'ticks':ticks,'sha256':value};
  }
  File('D:/workspace/eagler/artifacts/native-launcher-85941f0d56/hover-detail-perf/hash-probe.json').writeAsStringSync(const JsonEncoder.withIndent('  ').convert(output));
  print(jsonEncode(output));
}
