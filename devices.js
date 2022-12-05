import WebSocket, { WebSocketServer } from "ws";

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
      const { device, ...json } = JSON.parse(string)
      //console.log(wss.clients);
      console.log(json);
      devices[device] = {...json, timestamp: new Date()}
      //send back devices to all other clients?
      wss.clients.forEach(client => {
        if (client !== ws && client.readyState === WebSocket.OPEN) {
          client.send(JSON.stringify({...devices[device], device}));
        }
      });
    });

    //this was just a test...
    // ws.send(JSON.stringify({
    //   device: "artbox",
    //   green: 179,
    //   blue: 0,
    //   red: 30,
    //   color: 'green',
    //   isOn: false,
    //   initial: true
    // }));
  });
}