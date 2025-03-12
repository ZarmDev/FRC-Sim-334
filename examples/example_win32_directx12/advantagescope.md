gyroTopic.subscribe((value) => {
  console.log(`Got Gyro Value: ${value}`);
});

subscribe((value) => {
  console.log(`Got Gyro Value: ${value}`);
}, {}, getNextSubuid, true);

messenger.subscribe({
topics: /MyTable/Gyro, 
subuid,
options ({})
});

sendTextFrame({
  method: 'subscribe',
  {
topics: /MyTable/Gyro, 
subuid,
options ({})
})

if (this.ws && this.ws.readyState === WebSocket.OPEN) {
      this.ws.send(
        JSON.stringify([
          {
            method: method,
            params: params
          }
        ])
      );
    }

websocket.send(JSON.stringify([
  {
  method: 'subscribe',
  params: {
topics: /MyTable/Gyro, 
subuid,
options ({})
}
}
])

import { Decoder, Encoder } from "@msgpack/msgpack";

const typestrIdxLookup: { [id: string]: number } = {
  boolean: 0,
  double: 1,
  int: 2,
  float: 3,
  string: 4,
  json: 4,
  raw: 5,
  rpc: 5,
  msgpack: 5,
  protobuf: 5,
  "boolean[]": 16,
  "double[]": 17,
  "int[]": 18,
  "float[]": 19,
  "string[]": 20
};
