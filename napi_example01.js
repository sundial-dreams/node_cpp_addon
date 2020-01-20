const { filter, hello, createObject, Person, createPerson } = require("./build/Release/napi_addon01");
console.log(hello());
console.log(filter(["", 2, false], v => !!v));
console.log(createObject("name", 21));
console.log(new Person("dpf", 21));
console.log(createPerson("daydream", 21));
