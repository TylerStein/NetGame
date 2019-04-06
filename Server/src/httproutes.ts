import * as express from 'express';
const router = express.Router();

router.all('/', (req, res, next) => {
    console.log(`[HTTP ${req.method}] from ${req.ip}`, req.body);
    res.status(200).send('OK');
});

module.exports = router;