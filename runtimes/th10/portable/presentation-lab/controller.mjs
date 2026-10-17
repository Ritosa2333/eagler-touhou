import {PresentationLabControllerCore} from '../../third_party/eagler-common/testkit/presentation-lab/controller-core.mjs';
import {Th10RuntimeDriver,Th10ObservationAdapter} from './adapter.mjs';
export class LabController extends PresentationLabControllerCore{
  constructor(runtime,identity){const driver=new Th10RuntimeDriver(runtime,identity),observer=new Th10ObservationAdapter(runtime);super({driver,observer,identity});this.runtime=runtime;this.core=runtime.core;}
}
