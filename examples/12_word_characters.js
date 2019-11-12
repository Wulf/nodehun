const Nodehun = require('bindings')('Nodehun')
const dictionaries = require('./dictionaries')

const nodehun = new Nodehun(dictionaries.en_US.affix, dictionaries.en_US.dictionary)



const wordCharacters = nodehun.getWordCharacters()
console.log('Dictionary\'s word characters are:', wordCharacters) // => 0123456789'.-’

const wordCharactersUTF16 = nodehun.getWordCharactersUTF16()
console.log('Dictionary\'s word characters (UTF-16) are:', wordCharactersUTF16) // => '-.0123456789’