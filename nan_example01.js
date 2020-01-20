const { filter, hello, createObject } = require("./build/Release/nan_addon01");
console.log(hello());
console.log(filter(["aaa", 2, 2, "bb"], (v) => typeof v === "number"));
console.log(createObject("daydream", 21).getAll());