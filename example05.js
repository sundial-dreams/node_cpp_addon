const { Person } = require("./build/Release/addon05");
const person = new Person("dengpengfei", 21);
console.log(person.getName());
console.log(person.getAge());

const person1 = Person("sundial-dreams", 21); // 直接用
console.log(person1.getName());
console.log(person1.getAge());
