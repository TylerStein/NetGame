export enum EMessageType {
    NULL = 0,
    CONN = 1,
    KILL = 2,
    DATA = 3,
    LIST = 4
}

export interface Message {
    messageType: EMessageType;
    payload: string;
}

export class TCPInterpreter {
    /**
     * Possible Message Types:
     * CONN:<ID>
     * DATA:<PAYLOAD>
     * LIST:<TYPE>
     * KILL:<ID>
     */
    private testCONN = (value: string) => /CONN:(\w+)/g.exec(value);
    private testKILL = (value: string) => /KILL:(\w+)/g.exec(value);
    private testLIST = (value: string) => /LIST:(\w+)/g.exec(value);
    private testDATA = (value: string) => /DATA:(.+)/g.exec(value);

    constructor() {
        //
    }

    Interpret(data: string): Message {
        let x = this.testCONN(data);
        if (x && x[1]) {
            return {
                messageType: EMessageType.CONN,
                payload: x[1]
            }
        }

        x = this.testKILL(data);
        if (x && x[1]) {
            return {
                messageType: EMessageType.KILL,
                payload: x[1]
            }
        }

        x = this.testLIST(data);
        if (x && x[1]) {
            return {
                messageType: EMessageType.LIST,
                payload: x[1]
            }
        }

        x = this.testDATA(data);
        if (x && x[1]) {
            return {
                messageType: EMessageType.DATA,
                payload: x[1]
            }
        }

        return {
            messageType: EMessageType.NULL,
            payload: data
        };
    }

    private testDataAgainstRegexp(value: string, exp: RegExp) {
        return exp.exec(value);
    }
}
