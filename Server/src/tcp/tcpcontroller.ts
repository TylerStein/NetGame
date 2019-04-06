import { TCPClient } from './tcpclient';
import { TCPServer, SocketData, SocketIdentifier, SocketError } from './tcpserver';
import { TCPInterpreter, EMessageType } from './tcpinterpreter';
import { Subject } from 'rxjs';

export class TCPController {
    private clients: TCPClient[] = [];
    server: TCPServer;
    interpreter: TCPInterpreter;

    constructor(serverPort = 3001, serverHost = '127.0.0.1') {
        this.server = new TCPServer(serverPort, serverHost);
        this.interpreter = new TCPInterpreter();

        this.server.onSocketConnect$.subscribe((socketIdenitifer: SocketIdentifier) => {
            let existingClient = this.clients.find((value) => value.clientIP == socketIdenitifer.remoteAddress && value.clientPort == socketIdenitifer.remotePort);
            if (!existingClient) {
                console.log(`[TCPController] Created connection with new client (${socketIdenitifer.remoteAddress}:${socketIdenitifer.remotePort})`);
                let client: TCPClient = { 
                    clientIP: socketIdenitifer.remoteAddress,
                    clientPort: socketIdenitifer.remotePort,
                    isConnected: true,
                    clientID: null
                };
                this.clients.push(client);
            } else {
                console.warn(`[TCPController] Received connection request for existing client (${existingClient.clientIP}:${existingClient.clientPort})`);
            }

        });

        this.server.onSocketClose$.subscribe((socketIdentifier: SocketIdentifier) => {
            let clientIndex = this.clients.findIndex((value) => value.clientIP == socketIdentifier.remoteAddress && value.clientPort == socketIdentifier.remotePort);
            let existingClient = this.clients[clientIndex];
            if (existingClient) {
                console.log(`[TCPController] Closing connection with existing client (${existingClient.clientIP}:${existingClient.clientPort})`);
                this.clients.splice(clientIndex, 1);
            } else {
                console.warn(`[TCPController] Received close request for missing client (${socketIdentifier.remoteAddress}:${socketIdentifier.remotePort})`);
            }
        });

        this.server.onSocketData$.subscribe((socketData: SocketData) => {
            let existingClient = this.clients.find((value) => value.clientIP == socketData.remoteAddress && value.clientPort == socketData.remotePort);
            if (existingClient) {
              //  console.log(`[TCPController] Received data for existing client (${existingClient.clientIP}:${existingClient.clientPort})`, socketData.data);
                let x_raw: string= socketData.data.substr(0, 8);
                let y_raw: string = socketData.data.substr(8, 8);
                let name_raw: string = socketData.data.substr(16);

                let x_buf = Buffer.from(x_raw, 'hex');
                let x = x_buf.readInt32BE(0);

                let y_buf = Buffer.from(y_raw, 'hex');
                let y = y_buf.readInt32BE(0);

                let name_buf = Buffer.from(name_raw, 'hex');
                let name = name_buf.toString('utf8');

                existingClient.clientID = name;

                let outBuffer = Buffer.alloc(16, '\0', 'hex');
                outBuffer.writeInt32BE(x, 0);
                outBuffer.writeInt32BE(y, 4)
                outBuffer.write(name, 8, 'utf8');

                
                if (name.length > 8) name = name.substr(0, 8);
                else name = name.padEnd(8, '\0');

                let outMessage = outBuffer.toString('hex', 0, 16);

                console.log(`${name} (${x}, ${y})`)

                this.clients.forEach((client) => {
                    if (client.clientIP != existingClient.clientIP && client.clientPort != existingClient.clientPort) {
                        this.SendMessage(client, outMessage);
                    }
                })

              //  console.log(`IN => ${x}${y}${name} | ${socketData.data}`)
            } else {
                console.warn(`[TCPController] Received data request from missing client (${socketData.remoteAddress}:${socketData.remotePort})`, socketData.data);
            }
        });

        this.server.onSocketError$.subscribe((socketError: SocketError) => {
            let existingClient = this.clients.find((value) => value.clientIP == socketError.remoteAddress && value.clientPort == socketError.remotePort);
            if (existingClient) {
                console.log(`[TCPController] Received error for existing client (${existingClient.clientIP}:${existingClient.clientPort})`, socketError.error);
            } else {
                console.warn(`[TCPController] Received error from missing client (${socketError.remoteAddress}:${socketError.remotePort})`, socketError.error);
            }
        })

        this.server.Listen();
    }

    GetClients() {
        return this.clients;
    }

    GetClientById(clientId: string) {
        return this.clients.find((value) => value.clientID == clientId);
    }

    AddClient(client: TCPClient) {
        this.clients.push(client);
    }

    BroadcastMessage(message: string) {
        this.clients.forEach((client) => this.SendMessage(client, message));
    }

    SendMessage(client: TCPClient, message: string) {
        if (!client || !client.isConnected) return;
        let socket = this.server.FindSocket(client.clientIP, client.clientPort);
        if (!socket) return;
        const msgBuffer: Buffer = Buffer.from(message);
        socket.write(msgBuffer);
    }

    private handleIncomingData(client: TCPClient, data: string) {
        let message = this.interpreter.Interpret(data);
        let displayName = client.clientID || `${client.clientIP}:${client.clientPort}`;
        switch(message.messageType) {
            case EMessageType.CONN:
                client.clientID = message.payload;
                console.log(`[TCPController] Client CONN signal from ${client.clientIP}:${client.clientPort} set ID to ${client.clientID}`);
                break;
            case EMessageType.DATA:
                console.log(`[TCPController] Client DATA signal from ${client.clientID || (client.clientIP + ':' + client.clientPort)}`, message.payload);
               // this.BroadcastMessage(`${displayName} SAYS ${message.payload}`);
                break;
            case EMessageType.KILL:
                console.log(`[TCPController] Client KILL signal from ${displayName}`);
                break;
            case EMessageType.LIST:
                console.log(`[TCPController] Client LIST signal from ${displayName}`);
                let listMessage = 'CLIENTS LIST: ' + this.clients.map((c) => c.clientID || (c.clientIP + ':' + c.clientPort)).join(', ');
               // this.SendMessage(client, listMessage);
                break;
            case EMessageType.NULL:
                console.log(`[TCPController] Client INVALID SIGNAL from ${displayName}`);
                break;
            default:
                console.log(`[TCPController] Client UNHANDLED SIGNAL from ${displayName}`);
                break;
        }
    }
}