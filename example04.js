const { Person, createObject, createObject2 } = require("./build/Release/addon04");
console.log(new Person("sundial-dream", 21));
console.log(createObject("daydream", 21));
console.log(createObject2("dengpengfei", 21).getAll());