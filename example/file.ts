
const { 
  Wechaty, 
  Message,
  ScanStatus,
  log,
} = require('wechaty')


import { PuppetWork } from './puppet-work'

let puppetwork = new PuppetWork({})

const welcome = `
=============== Powered by Wechaty ===============
-------- https://github.com/chatie/wechaty --------
I'm a bot, I can save file to local for you!
__________________________________________________
Please wait... I'm trying to login in...
`
console.log(welcome)

const bot = new Wechaty({
    name: 'ding-dong-bot',
    puppet: puppetwork,
    // Set as above, or set using environment variable WECHATY_PUPPET
})
bot.on('login',   onLogin)
bot.on('logout',  onLogout)
bot.on('message', onMessage)
bot.on('error',   onError)

bot.start()
.catch(console.error)

function onLogin (user) {
  console.log(`${user} login`)
}

function onLogout (user) {
  console.log(`${user} logout`)
}

function onError (e) {
  console.error(e)
}

async function onMessage(msg) {
  console.log(`RECV: ${msg}`)

  if (msg.type() !== Message.Type.Text) {
    const file = await msg.toFileBox()
    const name = file.name
    console.log('Save file to: ' + name)
    file.toFile(name, true)
  }
}