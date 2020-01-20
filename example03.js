const { eval, add, filter, createObject } = require("./build/Release/addon03");
const code = `
    let arr = ["hello", "world"];
    print(CONST);
    for(let v of arr) {
        print(v);
    }
`;
console.log(eval(code));
console.log(add(222, 222));
console.log(filter(["aaa", 2, "bb", 222], (v, i) => typeof v === "string"));
console.log(createObject("sundial-dreams", 21));