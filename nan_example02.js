const { Person } = require("./build/Release/nan_addon02");
const p = new Person("sundial-dreams", 21);
console.log(p);
console.log(p.getName());
console.log(p.getAge());