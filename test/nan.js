var affbuf = Buffer.from(`SET UTF-8\nTRY esianrtolcdugmphbyfvkwzESIANRTOLCDUGMPHBYFVKWZ\nSFX X Y 1\nSFX X 0 able . ds:able`)
var dictbuf = Buffer.from(`1\ndrink/X po:verb`)

console.log('Nodehun NAN Implementation Tests');
console.log('--------------------------------');
var Nodehun = require('..');
console.log('constructor:',Nodehun);
const instance = new Nodehun(affbuf, dictbuf)
console.log('instance.test:',instance.test);
console.log('instance.correct:',instance.stem());
