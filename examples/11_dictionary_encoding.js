const Nodehun = require('bindings')('Nodehun')
const dictionaries = require('./dictionaries')

const nodehun = new Nodehun(dictionaries.en_US.affix, dictionaries.en_US.dictionary)



const encoding = nodehun.getDictionaryEncoding()
console.log('Dictionary\'s encoding is:', encoding) // => 'UTF-8'
