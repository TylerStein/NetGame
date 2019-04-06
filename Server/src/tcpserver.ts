import * as net from 'net';

const expressions = {
    conn: (x: string) => /CONN:(\w+)/g.exec(x),
    kill: (x: string) => /KILL:(\w+)/g.exec(x),
    data: (x: string) => /(\w+):(.+)/g.exec(x)
}

const resolveData = (data: any) => {
    let result = {
        type: 'NULL',
        client: <any>null,
        data: <any>null
    };
    let rgx = expressions.conn(data);
    if (rgx && rgx[1]) {
        result.type = 'CONN';
        result.client = null;
        result.data = rgx[1];
        return result;
    }
    rgx = expressions.kill(data);
    if (rgx && rgx[1]) {
        result.type = 'KILL';
        result.client = null;
        result.data = rgx[1];
        return result;
    }
    rgx = expressions.data(data);
    if (rgx && rgx [1] && rgx[2]) {
        result.type = 'DATA';
        result.client = rgx[1];
        result.data = rgx[2];
        return result;
    }
    console.error('Unrecognized data!', data);
    return result;
}

const sigterm = '\0';
const write = (socket: net.Socket, data: any) => {
   // let buf = Buffer.from(data.substring(0, 511) + sigterm, 0, 512).toString();
    console.log('writing data', data);
    socket.write(`${data.substring(0, 512)}`);
}

module.exports = (port: number, host: string) => {
    let sockets: net.Socket[] = [];
    let clients: any = {};
    const server = net.createServer();
    server.listen(port, host, () => {
        console.log(`TCP Server is running on port ${port} with host ${host}`);
    });

    server.on('connection', (socket) => {
        console.log(`[TCP] Connection: ${socket.remoteAddress}:${socket.remotePort}`);
        sockets.push(socket);

        socket.on('data', (data) => {
            console.log(`[TCP] ${socket.remoteAddress}: ${data.toString()}`);
            const decoded = data.toString();
            const resolved = resolveData(decoded);
            let client;

            switch (resolved.type) {
                case 'CONN':
                    client = clients[resolved.data];
                    if (client) {
                        console.warn(`Received conn signal from already registered client ${resolved.data}`);
                        client.alive = true;
                        write(socket, `STATUS:OK,OPERATION:CLIENT_ALIVE`);
                    } else {
                        console.log(`Received conn signal from new client ${resolved.data}`);
                        clients[resolved.data] = { alive: true, history: [] };
                        write(socket, `STATUS:OK,OPERATION:CLIENT_CREATED`);
                    }
                    break;
                case 'KILL':
                    client = clients[resolved.data];
                    if (client) {
                        client.alive = false;
                        console.log(`Killing client ${resolved.data} with history: `, JSON.stringify(client.history));
                        client.history = [];
                        write(socket, `STATUS:OK,OPERATION:CLIENT_KILLED`);
                    } else {
                        console.warn(`Received kill signal from unregistered client ${resolved.data}`);
                        write(socket, `STATUS:INVALID_REQUEST,REASON:INVALID_CLIENT`);
                    }
                    break;
                case 'DATA':
                    client = clients[resolved.client];
                    if (client) {
                        if (client.alive) {
                            console.log(`Received data for living client ${client}: ${resolved.data}`);
                            client.history.push(resolved.data);
                            write(socket, `STATUS:OK`);
                            setTimeout(() => {
                                write(socket, `ECHO:${resolved.data.substring(0, 500)}`);
                            }, 2500);
                        } else {
                            console.warn(`Received data for killed client ${client}: ${resolved.data}`);
                            write(socket, `STATUS:INVALID_REQUEST,REASON:KILLED_CLIENT`);
                        }
                    } else {
                        console.warn(`Received data for unregistered client ${resolved.client}: ${resolved.data}`);
                        write(socket, `STATUS:INVALID_REQUEST,REASON:INVALID_CLIENT`);
                    }
                    break;
                default:
                    console.warn(`Received unrecognized data ${data}`);
                    write(socket, `STATUS:INVALID_REQUEST,REASON:UNRECOGNIZED_FN`);
                    break;
            }

        });

        socket.on('close', (hadError) => {
            let socketIndex = sockets.findIndex((o) => (o.remoteAddress == socket.remoteAddress && o.remotePort == socket.remotePort));
            if (socketIndex !== -1) sockets.splice(socketIndex, 1);
            console.log(`[TCP] ${socket.remoteAddress}: Closed (error = ${hadError})`);
        });
    });    
}
