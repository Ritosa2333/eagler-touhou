import {Transform} from 'node:stream';

export default class OracleTestEventReporter extends Transform {
  constructor() {
    super({writableObjectMode:true});
  }

  _transform(event,_encoding,callback) {
    try {
      this.push(JSON.stringify(event)+'\n');
      callback();
    } catch (error) {
      callback(error);
    }
  }
}
