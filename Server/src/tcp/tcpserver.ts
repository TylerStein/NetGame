import { TCPClient } from './tcpclient';
import { Socket, Server, createServer } from 'net';
import { Subject } from 'rxjs';

export interface SocketIdentifier {
    remoteAddress: string;
    remotePort: number;
}

export interface SocketData extends SocketIdentifier {
    data: string;
}

export interface SocketError extends SocketIdentifier {
    error: string;
}

export class TCPServer {
    public onSocketConnect$: Subject<SocketIdentifier> = new Subject<SocketIdentifier>();
    public onSocketClose$: Subject<SocketIdentifier> = new Subject<SocketIdentifier>();
    public onSocketData$: Subject<SocketData> = new Subject<SocketData>();
    public onSocketError$: Subject<SocketError> = new Subject<SocketError>();

    private sockets: Socket[] = [];
    private server: Server;

    private port: number;
    private host: string;

    private listening: boolean;

    constructor(port: number, host: string) {
        this.port = port;
        this.host = host;
    }

    IsListening() {
        return this.listening;
    }

    FindSocket(address: string, port: number) {
        return this.sockets.find((value) => value.remoteAddress == address && value.remotePort == port);
    }

    Listen() {
        if (this.listening) return;
        this.server = createServer();
        this.server.listen(this.port, this.host, () => {
            this.listening = true;
            console.log(`[TCPServer] Listening (${this.host}:${this.port})`);
        });

        this.server.on('close', () => {
            console.log(`[TCPServer] Closing (${this.host}:${this.port})`);
            this.listening = false;
            this.sockets.forEach((socket) => {
                this.onSocketClose$.next({
                    remoteAddress: socket.remoteAddress,
                    remotePort: socket.remotePort
                });
            })
            this.sockets = [];
            console.log(`[TCPServer] Connection closed (${this.host}:${this.port})`);
        });

        this.server.on('connection', (socket) => {
            console.log(`[TCPServer] New socket connection ${socket.remoteAddress}:${socket.remotePort}`);
            this.sockets.push(socket);
            this.onSocketConnect$.next({
                remoteAddress: socket.remoteAddress,
                remotePort: socket.remotePort
            });

            socket.on('data', (data) => {
               // console.log(`[TCPServer] Received data from ${socket.remoteAddress}:${socket.remotePort}`);
                this.onSocketData$.next({
                    remoteAddress: socket.remoteAddress,
                    remotePort: socket.remotePort,
                    data: data.toString()
                });
            });

            socket.on('close', (hadError) => {
                if (hadError) console.log(`[TCPServer] Socket closed with error ${socket.remoteAddress}:${socket.remotePort}`);
                else console.log(`[TCPServer] Socket closed ${socket.remoteAddress}:${socket.remotePort}`);
                let socketIndex = this.sockets.findIndex((value) => value.remoteAddress == socket.remoteAddress && value.remotePort == socket.remotePort);
                if (socketIndex !== -1) this.sockets.splice(socketIndex, 1);
                this.onSocketClose$.next({
                    remoteAddress: socket.remoteAddress,
                    remotePort: socket.remotePort
                });
            });

            socket.on('error', (error) => {
                if (error) console.log(`[TCPServer} Socket error ${socket.remoteAddress}:${socket.remotePort}`, error);
                this.onSocketError$.next({
                    remoteAddress: socket.remoteAddress,
                    remotePort: socket.remotePort,
                    error: error.message
                });
            })
        });
    }
}
