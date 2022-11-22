import { Router } from "express"
const router = Router()
import { config as dotenvConfig } from "dotenv"
import axios from "axios"
dotenvConfig()
const wait = (amount = 0) => new Promise(resolve => setTimeout(resolve, amount));
// /status should be able to tell us? maybe?
let rainbowOn = {
  troll: false,
  artbox: false,
}

//todo: all this stuff should hold this...
let currentColors = {
  troll: null,
  artbox: null,
}

//set up a colors object that has the color name as the key and the rgb values as the value
const colors = {
  red: [179,0,0],
  pink: [179,0,45],
  magenta: [179,0,146],
  purple: [80,0,179],
  blue: [0,0,179],
  lightblue: [0,110,179],
  aqua: [0,179,143],
  green: [30,179,0],
  yellow: [179,164,0],
  orange: [179,18,0],
  lime: [7, 255, 110],
}

const rainbow = [
  [179,0,0],
  [179,0,0],
  [179,0,9],
  [179,0,18],
  [179,0,27],
  [179,0,36],
  [179,0,45],
  [179,0,45],
  [179,0,45],
  [179,0,54],
  [179,0,62],
  [179,0,71],
  [179,0,77],
  [179,0,86],
  [179,0,95],
  [179,0,104],
  [179,0,113],
  [179,0,122],
  [179,0,131],
  [179,0,140],
  [179,0,146],
  [179,0,146],
  [179,0,146],
  [179,0,155],
  [179,0,164],
  [179,0,173],
  [176,0,179],
  [167,0,179],
  [158,0,179],
  [149,0,179],
  [140,0,179],
  [131,0,179],
  [122,0,179],
  [116,0,179],
  [107,0,179],
  [98,0,179],
  [89,0,179],
  [80,0,179],
  [80,0,179],
  [80,0,179],
  [71,0,179],
  [62,0,179],
  [54,0,179],
  [48,0,179],
  [39,0,179],
  [30,0,179],
  [21,0,179],
  [12,0,179],
  [3,0,179],
  [0,0,179],
  [0,0,179],
  [0,0,179],
  [0,0,179],
  [0,6,179],
  [0,12,179],
  [0,21,179],
  [0,30,179],
  [0,36,179],
  [0,45,179],
  [0,54,179],
  [0,60,179],
  [0,68,179],
  [0,77,179],
  [0,86,179],
  [0,92,179],
  [0,101,179],
  [0,110,179],
  [0,110,179],
  [0,110,179],
  [0,119,179],
  [0,125,179],
  [0,134,179],
  [0,143,179],
  [0,149,179],
  [0,158,179],
  [0,167,179],
  [0,176,179],
  [0,179,176],
  [0,179,167],
  [0,179,158],
  [0,179,149],
  [0,179,143],
  [0,179,143],
  [0,179,143],
  [0,179,134],
  [0,179,125],
  [0,179,119],
  [0,179,110],
  [0,179,101],
  [0,179,95],
  [0,179,86],
  [0,179,77],
  [0,179,68],
  [0,179,60],
  [0,179,54],
  [0,179,45],
  [0,179,36],
  [0,179,30],
  [0,179,21],
  [0,179,12],
  [0,179,3],
  [0,179,0],
  [0,179,0],
  [0,179,0],
  [3,179,0],
  [12,179,0],
  [21,179,0],
  [30,179,0],
  [30,179,0],
  [39,179,0],
  [48,179,0],
  [54,179,0],
  [62,179,0],
  [71,179,0],
  [80,179,0],
  [89,179,0],
  [98,179,0],
  [107,179,0],
  [116,179,0],
  [122,179,0],
  [131,179,0],
  [140,179,0],
  [149,179,0],
  [158,179,0],
  [167,179,0],
  [176,179,0],
  [179,173,0],
  [179,164,0],
  [179,164,0],
  [179,164,0],
  [179,155,0],
  [179,149,0],
  [179,140,0],
  [179,131,0],
  [179,122,0],
  [179,113,0],
  [179,104,0],
  [179,95,0],
  [179,86,0],
  [179,80,0],
  [179,71,0],
  [179,62,0],
  [179,54,0],
  [179,45,0],
  [179,36,0],
  [179,27,0],
  [179,18,0],
  [179,18,0],
  [179,18,0],
  [179,18,0],
  [179,12,0],
  [179,0,0],
  [179,0,0],
]

// let x = Object.entries(rainbow.reduce((obj, color) => {
//   if(typeof obj[color] === "undefined") {
//     return {...obj, [color]: 1}
//   } else {
//     return {...obj, [color]: obj[color] + 1}
//   }
// }, {})).filter(([key, value]) => value > 1).map(([color]) => `<span style="color: white; display: block; width: 100%; margin-top: 10px; height: 30px; background-color: rgb(${color})">${color}</span>`).join(" ")



router.get("/", async(req, res) => {
  //await axios.get(`http://70.191.119.213:3001/`)
  //console.log("hello from led root?");
  //console.log("hello from led root??", req);
  res.send("Hello led World!");
})

//done
router.get("/colors", async(req, res) => {
  res.send(colors);
})

//todo:
router.get("/:device/on", async(req, res) => {
  const { url, body, device } = getDeviceData(req)
  rainbowOn[device] = false

  try {
    const { data } = await axios.get(`${url}/status`)
    const { color: currentColor, red, green, blue } = data

    if(currentColor === "rainbow") {
      cycleThroughRainbow({ url, device }, [red, green, blue])
    } else {
      let [ red, green, blue ] = colors[currentColor] ?? colors["red"]
      
      if(device !== "artbox") {
        [ red, green, blue ] = getInvertedColors([red, green, blue])
      }
      axios.post(`${url}/rgb`, { red, green, blue, name: currentColor })
    }
    res.send(data);
  } catch(error) {
    console.log(error);
    res.send({ error });
  }

  
  //res.send(data);
})

//done
router.get("/:device/off", async(req, res) => {
  const { url, body, device } = getDeviceData(req)
  rainbowOn[device] = false

  try {
    //DO NOT CHANGE - if this was run through regular rgb then previous and color vars on board will get messed up
    const { data } = await axios.get(`${url}/off`)
    res.send(data);
  } catch(error) {
    console.log(error);
    res.send({ error });
  }
})

//ok
router.get("/:device/rainbow", async(req, res) => {
  const { url, body, device } = getDeviceData(req)
  setDeviceData(req)
  await wait(100)
  cycleThroughRainbow({ url, device })

  res.send({});
})

//done
router.get("/:device/rainbow/start", async(req, res) => {
  const { url, body, device } = getDeviceData(req)
  cycleThroughRainbow({ url, device }, Object.values(req.query).map(value => parseInt(value)))
  res.send({});
})

//done
router.get("/:device/rainbow/stop", async(req, res) => {
  let { device } = req.params
  rainbowOn[device] = false
  res.send({})
})

//done
router.get("/:device/status", async(req, res) => {
  const { url, body, device } = getDeviceData(req) 

  try {
    const { data } = await axios.get(`${url}/status`)
    res.send(data);
  } catch(error) {
    console.log(error);
    res.send({ error });
  }
})

//done
router.post("/:device/rgb", async(req, res) => {
  const { url, body, device } = getDeviceData(req)
  rainbowOn[device] = false
  try {
    const { data } = await axios.post(`${url}/rgb`, {...body, name: "custom"})
    res.send(data);
  } catch(error) {
    console.log(error);
    res.send({ error });
  }
})

//done
router.get("/:device/rgb", async(req, res) => {
  const { url, body, device } = getDeviceData(req)
  const [ red, green, blue ] = colors[req.query.color]
  res.send({ red, green, blue })
})

//done
router.get("/:device/:color", async(req, res) => {
  const { url, body, device } = getDeviceData(req) 
  setDeviceData(req)
  console.log("i have heard?");

  try {
    const { data } = await axios.post(`${url}/rgb`, {...body, name: req.params.color})
    console.log("im back");
    res.send(data);
  } catch(error) {
    console.log(error);
    res.send({ error });
  }
})

function setDeviceData({ params }, isRainbow = false) {
  const { device, color } = params

  currentColors[device] = !isRainbow ? color : "rainbow"
  rainbowOn[device] = isRainbow
}

function getDeviceData({ params, body }) {
  let { color, device } = params
  let url = `${process.env.ROUTER}:${device === "artbox" ? process.env.ARTBOX_ESP8266 : process.env.TROLL_HAIR_ESP8266}`

  if(typeof color !== "undefined") {
    let [ red, green, blue ] = colors[color.toLowerCase()]
    if(device !== "artbox") {
      [ red, green, blue ] = getInvertedColors([red, green, blue])
    }
    return { url, body: { red, green, blue }, device }
  } else if(Object.entries(body).length > 0) {
    //means it probably came from the color picker /rgb
    let { red, green, blue } = body
    console.log(red, green, blue);
    if(device !== "artbox") {
      [ red, green, blue ] = getInvertedColors([red, green, blue])
    }
    return { url, body: { red, green, blue }, device }
  } else {
    return { url, device }
  }
}

async function cycleThroughRainbow({ url, device }, existingColors = null) {
  rainbowOn[device] = true
  currentColors[device] = "rainbow"
  let index = 0
  if(existingColors !== null) {
    index = rainbow.findIndex(rgb => {
      return JSON.stringify(rgb) === JSON.stringify(existingColors)
    })
  }
  //console.log(currentColors);

  while(rainbowOn[device]) {
    if(index === rainbow.length - 1) {
      index = 0
    }

    let [ red, green, blue ] = rainbow[index]
    if(device !== "artbox") {
      [ red, green, blue ] = getInvertedColors([red, green, blue])
    }
    const { data } = await axios.post(`${url}/rgb`, { red, green, blue, name: "rainbow" })
    await wait(50)
    //console.log("i am cycling?");

    index++;
  }
}

function getInvertedColors(colors) {
  return colors.map(value => 255 - value)
}


export default router