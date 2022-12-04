import express from "express"
const app = express()
import { fileURLToPath } from "url"
import path from "path"
const { dirname } = path
import { WebSocketServer } from "ws";

const __filename = fileURLToPath(import.meta.url)
const __dirname = dirname(__filename)
const port = 3003;

import { config as dotenvConfig } from "dotenv"
import cors from "cors"
import kitchenLedsRouter from "./routes/kitchen-leds.js"
import ledsRouter from "./routes/leds.js"
app.use(express.json())
app.use(cors())

app.use(express.static(path.join(__dirname, "./static")))
// app.set("trust proxy", "loopback")
// app.use(function(req, res, next) {
//   console.log("inside middleware?");
//   if(!req.url.split("/").includes("kitchen-leds")) {
//     console.log("doing next");
//     next()
//     return
//   }

//   const ips = process.env.IPS.split(",")
//   console.log(req.ip)
//   if(ips.find(ip => req.ip.includes(ip))) {
//     next()
//   } else {
//     console.log("unauthorized", req.ip);
//     res.sendStatus(403)
//   }
// })

import axios from "axios"

app.get("/", async (req, res) => {
  //const test = await axios.get('http://70.191.119.213:3003/')
  //const test = await axios.get('https://jsonplaceholder.typicode.com/todos/1')
  console.log("ive heard something?");
  res.send("Hello from get root?!");
})

app.post("/", (req, res) => {
  console.log(req.body);
  res.send("is buddy working??");
})

app.use("/kitchen-leds", kitchenLedsRouter)
app.use("/leds", ledsRouter)

app.listen(port, () => {
  console.log(`Example app listening on port ${port}`);
});


(async function () {
  kitchenLedsRouter.initKitchenLeds()
}());

const wss = new WebSocketServer({port: 3008})
let devices = {}

wss.on("connection", function connection(ws) {
  console.log("i got a connection?");
  ws.on("message", (data) => {
    console.log('received: %s', data);
  });

  ws.send('something');
});




