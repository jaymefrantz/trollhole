import { WebSocketServer } from 'ws'

export default function initWebsocket(server) {
  console.log("initing websocket?");
  const wss = new WebSocketServer({ server });
  let devices = {}

  wss.on("connection", (ws) => {
    console.log("i got a connection?");
    ws.on("message", (data) => {
      console.log('received: %s', data);
    });

    ws.send('hello from node server...?');
  });
}