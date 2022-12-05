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

  wss.on("connection", ws => {
    //console.log("i got a connection?");
    ws.on("message", string => {
      const { device, method = "update", ...json } = JSON.parse(string)

      if(method === "get") { //app calls this on connection to socket
        Object.entries(devices).forEach(([device, data]) => {
          ws.send(JSON.stringify({ device, ...data }))
        })
      } else {
        console.log(json);
        devices[device] = {...json, timestamp: new Date()}
        //send back devices to all other clients?
        wss.clients.forEach(client => {
          //if (client !== ws && client.readyState === WebSocket.OPEN) {
            client.send(JSON.stringify({...devices[device], device}));
          //}
        });
      }
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