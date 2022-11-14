import { Router } from "express"
const router = Router()
import { login } from "tplink-cloud-api";
import { config as dotenvConfig } from "dotenv"
dotenvConfig()
let tplink = null;
let kitchenLeds = null
let currentlyOn = false



router.initKitchenLeds = async function() {
  tplink = await login(process.env.TP_USERNAME, process.env.TP_PASSWORD);
  let deviceList = await tplink.getDeviceList();
  kitchenLeds = tplink.getHS100("kitchen lights")

  try {
    let response = await kitchenLeds.getSysInfo();
    currentlyOn = response.relay_state === 1
  } catch(e) {
    console.log("the kitchen lights are not online");
  }
}

router.get("/", async(req, res) => {
  if(kitchenLeds === null || tplink === null) return

  const brightness = parseInt(req.query.brightness)
  console.log(brightness);
  const hour = parseInt(new Date().toLocaleString("en-US", { timeZone: "America/New_York", hour12: false, hour: "2-digit" }).split(" ")[0])

  
  //not within the time so turn it off
  if(hour > 2 && hour < 10) {
    if(currentlyOn) {
      kitchenLeds.powerOff()
      currentlyOn = false
    }
  } else {
    //falls within normal hours
    if(brightness < 100) {
      if(!currentlyOn) {
        kitchenLeds.powerOn()
        currentlyOn = true
      }
    } else {
      if(currentlyOn) {
        kitchenLeds.powerOff()
        currentlyOn = false
      }
    }
  }

  res.send("Hello World!");
})

//used for debugging, don't actually need
router.get("/:state", async(req, res) => {
  if(req.params.state === "on") {
    console.log("on");
    kitchenLeds.powerOn()
  } else if(req.params.state === "off")  {
    console.log("off");
    kitchenLeds.powerOff()
  } else if(req.params.state === "toggle") {
    console.log("toggle");
    kitchenLeds.toggle()
  }
  
  res.send("Hello kitchen World!");
})

export default router