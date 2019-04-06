"use strict";
var __importStar = (this && this.__importStar) || function (mod) {
    if (mod && mod.__esModule) return mod;
    var result = {};
    if (mod != null) for (var k in mod) if (Object.hasOwnProperty.call(mod, k)) result[k] = mod[k];
    result["default"] = mod;
    return result;
};
Object.defineProperty(exports, "__esModule", { value: true });
const express = __importStar(require("express"));
const router = express.Router();
router.all('/', (req, res, next) => {
    console.log(`[HTTP ${req.method}] from ${req.ip}`, req.body);
    res.status(200).send('OK');
});
module.exports = router;
//# sourceMappingURL=httproutes.js.map