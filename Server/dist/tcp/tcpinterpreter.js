"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var EMessageType;
(function (EMessageType) {
    EMessageType[EMessageType["NULL"] = 0] = "NULL";
    EMessageType[EMessageType["CONN"] = 1] = "CONN";
    EMessageType[EMessageType["KILL"] = 2] = "KILL";
    EMessageType[EMessageType["DATA"] = 3] = "DATA";
    EMessageType[EMessageType["LIST"] = 4] = "LIST";
})(EMessageType = exports.EMessageType || (exports.EMessageType = {}));
class TCPInterpreter {
    constructor() {
        /**
         * Possible Message Types:
         * CONN:<ID>
         * DATA:<PAYLOAD>
         * LIST:<TYPE>
         * KILL:<ID>
         */
        this.testCONN = (value) => /CONN:(\w+)/g.exec(value);
        this.testKILL = (value) => /KILL:(\w+)/g.exec(value);
        this.testLIST = (value) => /LIST:(\w+)/g.exec(value);
        this.testDATA = (value) => /DATA:(.+)/g.exec(value);
        //
    }
    Interpret(data) {
        let x = this.testCONN(data);
        if (x && x[1]) {
            return {
                messageType: EMessageType.CONN,
                payload: x[1]
            };
        }
        x = this.testKILL(data);
        if (x && x[1]) {
            return {
                messageType: EMessageType.KILL,
                payload: x[1]
            };
        }
        x = this.testLIST(data);
        if (x && x[1]) {
            return {
                messageType: EMessageType.LIST,
                payload: x[1]
            };
        }
        x = this.testDATA(data);
        if (x && x[1]) {
            return {
                messageType: EMessageType.DATA,
                payload: x[1]
            };
        }
        return {
            messageType: EMessageType.NULL,
            payload: data
        };
    }
    testDataAgainstRegexp(value, exp) {
        return exp.exec(value);
    }
}
exports.TCPInterpreter = TCPInterpreter;
//# sourceMappingURL=tcpinterpreter.js.map