const { filter, createObject } = require("./build/Release/node_addon_api_addon01");
console.log(filter([1, 3, 4, 4, 4,], v => v === 4));
const obj = createObject("dpf", 21);
console.log(obj);
console.log(obj.getAll());