import {LabController} from './controller.mjs';

export default {
  game: 'th10',
  title: '帧间显微镜',
  dataLabel: '选择自己的 th10.dat',
  dataUrl: '/input/th10.dat',
  runtimeUrl: epoch => `/runtime/th10.html?managedData=1&gameGeneration=presentation-lab&runtimeEpoch=${epoch}&manual=1`,
  runtime: target => target.__th10Runtime,
  async configure(runtime) {
    await runtime.command({
      command: 'configure', language: 'ja', music: 'none',
      options: {touchEnabled: false},
      sharedResources: [{path: '/msgothic.ttc', url: '/msgothic.ttc'}],
    });
  },
  createController: (runtime, identity) => new LabController(runtime, identity),
  // Presentation Lab accepts the retail Replay payload directly.  The .rpyx
  // name is reserved for files that actually carry the touch-motion trailer.
  installReplay: (runtime, bytes) => runtime.command({command: 'write', path: 'replay/th10_01.rpy', bytes}),
};
