const express = require('express');
const routes = require('./httproutes');
module.exports = (port) => {
    const app = express();
    app.use(routes);
    app.listen(port, () => {
        console.log(`HTTP Server listening on port ${port}`);
        0;
    });
};
//# sourceMappingURL=httpserver.js.map