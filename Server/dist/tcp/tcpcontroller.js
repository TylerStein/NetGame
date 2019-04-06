"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const tcpserver_1 = require("./tcpserver");
const tcpinterpreter_1 = require("./tcpinterpreter");
class TCPController {
    constructor(serverPort = 3001, serverHost = '127.0.0.1') {
        this.clients = [];
        this.server = new tcpserver_1.TCPServer(serverPort, serverHost);
        this.interpreter = new tcpinterpreter_1.TCPInterpreter();
        this.server.onSocketConnect$.subscribe((socketIdenitifer) => {
            let existingClient = this.clients.find((value) => value.clientIP == socketIdenitifer.remoteAddress && value.clientPort == socketIdenitifer.remotePort);
            if (!existingClient) {
                console.log(`[TCPController] Created connection with new client (${socketIdenitifer.remoteAddress}:${socketIdenitifer.remotePort})`);
                let client = {
                    clientIP: socketIdenitifer.remoteAddress,
                    clientPort: socketIdenitifer.remotePort,
                    isConnected: true,
                    clientID: null
                };
                this.clients.push(client);
            }
            else {
                console.warn(`[TCPController] Received connection request for existing client (${existingClient.clientIP}:${existingClient.clientPort})`);
            }
        });
        this.server.onSocketClose$.subscribe((socketIdentifier) => {
            let clientIndex = this.clients.findIndex((value) => value.clientIP == socketIdentifier.remoteAddress && value.clientPort == socketIdentifier.remotePort);
            let existingClient = this.clients[clientIndex];
            if (existingClient) {
                console.log(`[TCPController] Closing connection with existing client (${existingClient.clientIP}:${existingClient.clientPort})`);
                this.clients.splice(clientIndex, 1);
            }
            else {
                console.warn(`[TCPController] Received close request for missing client (${socketIdentifier.remoteAddress}:${socketIdentifier.remotePort})`);
            }
        });
        this.server.onSocketData$.subscribe((socketData) => {
            let existingClient = this.clients.find((value) => value.clientIP == socketData.remoteAddress && value.clientPort == socketData.remotePort);
            if (existingClient) {
                //  console.log(`[TCPController] Received data for existing client (${existingClient.clientIP}:${existingClient.clientPort})`, socketData.data);
                let x_raw = socketData.data.substr(0, 8);
                let y_raw = socketData.data.substr(8, 8);
                let name_raw = socketData.data.substr(16);
                let x_buf = Buffer.from(x_raw, 'hex');
                let x = x_buf.readInt32BE(0);
                let y_buf = Buffer.from(y_raw, 'hex');
                let y = y_buf.readInt32BE(0);
                let name_buf = Buffer.from(name_raw, 'hex');
                let name = name_buf.toString('utf8');
                existingClient.clientID = name;
                let outBuffer = Buffer.alloc(16, '\0', 'hex');
                outBuffer.writeInt32BE(x, 0);
                outBuffer.writeInt32BE(y, 4);
                outBuffer.write(name, 8, 'utf8');
                if (name.length > 8)
                    name = name.substr(0, 8);
                else
                    name = name.padEnd(8, '\0');
                let outMessage = outBuffer.toString('hex', 0, 16);
                console.log(`${name} (${x}, ${y})`);
                this.clients.forEach((client) => {
                    if (client.clientIP != existingClient.clientIP && client.clientPort != existingClient.clientPort) {
                        this.SendMessage(client, outMessage);
                    }
                });
                //  console.log(`IN => ${x}${y}${name} | ${socketData.data}`)
            }
            else {
                console.warn(`[TCPController] Received data request from missing client (${socketData.remoteAddress}:${socketData.remotePort})`, socketData.data);
            }
        });
        this.server.onSocketError$.subscribe((socketError) => {
            let existingClient = this.clients.find((value) => value.clientIP == socketError.remoteAddress && value.clientPort == socketError.remotePort);
            if (existingClient) {
                console.log(`[TCPController] Received error for existing client (${existingClient.clientIP}:${existingClient.clientPort})`, socketError.error);
            }
            else {
                console.warn(`[TCPController] Received error from missing client (${socketError.remoteAddress}:${socketError.remotePort})`, socketError.error);
            }
        });
        this.server.Listen();
    }
    GetClients() {
        return this.clients;
    }
    GetClientById(clientId) {
        return this.clients.find((value) => value.clientID == clientId);
    }
    AddClient(client) {
        this.clients.push(client);
    }
    BroadcastMessage(message) {
        this.clients.forEach((client) => this.SendMessage(client, message));
    }
    SendMessage(client, message) {
        if (!client || !client.isConnected)
            return;
        let socket = this.server.FindSocket(client.clientIP, client.clientPort);
        if (!socket)
            return;
        const msgBuffer = Buffer.from(message);
        socket.write(msgBuffer);
    }
    handleIncomingData(client, data) {
        let message = this.interpreter.Interpret(data);
        let displayName = client.clientID || `${client.clientIP}:${client.clientPort}`;
        switch (message.messageType) {
            case tcpinterpreter_1.EMessageType.CONN:
                client.clientID = message.payload;
                console.log(`[TCPController] Client CONN signal from ${client.clientIP}:${client.clientPort} set ID to ${client.clientID}`);
                break;
            case tcpinterpreter_1.EMessageType.DATA:
                console.log(`[TCPController] Client DATA signal from ${client.clientID || (client.clientIP + ':' + client.clientPort)}`, message.payload);
                // this.BroadcastMessage(`${displayName} SAYS ${message.payload}`);
                break;
            case tcpinterpreter_1.EMessageType.KILL:
                console.log(`[TCPController] Client KILL signal from ${displayName}`);
                break;
            case tcpinterpreter_1.EMessageType.LIST:
                console.log(`[TCPController] Client LIST signal from ${displayName}`);
                let listMessage = 'CLIENTS LIST: ' + this.clients.map((c) => c.clientID || (c.clientIP + ':' + c.clientPort)).join(', ');
                // this.SendMessage(client, listMessage);
                break;
            case tcpinterpreter_1.EMessageType.NULL:
                console.log(`[TCPController] Client INVALID SIGNAL from ${displayName}`);
                break;
            default:
                console.log(`[TCPController] Client UNHANDLED SIGNAL from ${displayName}`);
                break;
        }
    }
}
exports.TCPController = TCPController;
//# sourceMappingURL=tcpcontroller.js.map