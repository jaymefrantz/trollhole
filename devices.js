import { WebSocketServer } from 'ws'

/*
  devices = {
  "artbox": {
    red, green, blue, name, isOn
  }
}
*/

export default function initWebsocket(server) {
  const wss = new WebSocketServer({ server });
  let devices = {}

  wss.on("connection", (ws) => {
    //console.log("i got a connection?");
    ws.on("message", string => {
      const json = JSON.parse(string)
      console.log(json);
      //devices[json.device] = {...json.data, timestamp: new Date()}
      //send back devices to all other clients?

    });

    //ws.send('hello from node server...?');
  });
}