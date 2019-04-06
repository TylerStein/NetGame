import { TCPController } from './tcp/tcpcontroller';

const tcpController = new TCPController(63000, '192.168.0.19');
let bounds = { xmin: 0, ymin: 0, xmax: 720, ymax: 512 };
let bots = [
    {
        id: 'BOT_0',
        x: Math.fround(Math.round(Math.random() * bounds.xmax)),
        y: Math.fround(Math.round(Math.random() * bounds.ymax)),
        xDir: Math.random() * 5 + 2.5,
        yDir: Math.random() * 5 + 2.5,
        speed: 100
    },
    {
        id: 'BOT_1',
        x: Math.fround(Math.round(Math.random() * bounds.xmax)),
        y: Math.fround(Math.round(Math.random() * bounds.ymax)),
        xDir: Math.random() * -5 + 2.5,
        yDir: Math.random() * -5 + 2.5,
        speed: 100
    },
    {
        id: 'BOT_2',
        x: Math.fround(Math.round(Math.random() * bounds.xmax)),
        y: Math.fround(Math.round(Math.random() * bounds.ymax)),
        xDir: Math.random() * -5 + 2.5,
        yDir: Math.random() * -5 + 2.5,
        speed: 100
    }
];

const sendUpdate = (id: string, x: number, y: number) => {
    console.log(`${id} (${x}, ${y})`);
    let messageBuffer: Buffer = Buffer.alloc(16, '\0', 'hex');
    messageBuffer.writeInt32BE(x, 0);
    messageBuffer.writeInt32BE(y, 4);
    messageBuffer.write(id, 8, 'utf8');

    if (id.length > 8) id = id.substr(0, 8);
    else id = id.padEnd(8, '\0');

    let output = messageBuffer.toString('hex', 0, 16);
    tcpController.BroadcastMessage(output);
}

const getInBounds = (x: number, y: number) => {
    let res = { x, y };

    if (x > bounds.xmax) res.x = bounds.xmin;
    else if (x < bounds.xmin) res.x = bounds.xmax;

    if (y > bounds.ymax) res.y = bounds.ymin;
    else if (y < bounds.ymin) res.y = bounds.ymax;

    return res;
}

const ms = 33;
const dt = 1 / ms;
bots.forEach((bot) => {
    setInterval(() => {
        bot.x += Math.random() * 2 - 1;
        bot.y += Math.random() * 2 - 1;

        if (bot.x > bounds.xmax) bot.x = 0;
        if (bot.y > bounds.ymax) bot.y = 0;

        if (bot.x < 0) bot.x = bounds.xmax;
        if (bot.y < 0) bot.y = bounds.ymax;

        sendUpdate(bot.id, bot.x, bot.y);
    }, ms);
})