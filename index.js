const { findSubStr, subStrCount, BigNumber } = require("./build/Release/addon");
console.log("subStr index is: ", findSubStr("abcabdacac", "cab"));
console.log("subStr count is: ", subStrCount("abababcda", "ab"));

let n = new BigNumber("9999");
n.add(n);
console.log("add: ", n.val());
n.multiply("12222");
console.log("multiply: ", n.val());
