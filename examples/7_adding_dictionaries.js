const Nodehun = require('bindings')('Nodehun')
const dictionaries = require('./dictionaries')

let nodehun

/**
 * Async : using the promise
 */
nodehun = new Nodehun(dictionaries.en_US.affix, dictionaries.en_US.dictionary)
console.log(nodehun.spellSync('colour')) // => false
nodehun
    .addDictionary(dictionaries.en_CA.dictionary)
    .then(() => {
        console.log(nodehun.spellSync('colour')) // => true, no longer incorrect
    })

/**
 * Async : using async/await
 */
nodehun = new Nodehun(dictionaries.en_US.affix, dictionaries.en_US.dictionary)
async function generate() {
    console.log(nodehun.spellSync('colour')) // => false
    await nodehun.addDictionary(dictionaries.en_CA.dictionary)
    console.log(nodehun.spellSync('colour')) // => true, no longer incorrect
}
generate()

/**
 * Sync
 */
nodehun = new Nodehun(dictionaries.en_US.affix, dictionaries.en_US.dictionary)
console.log(nodehun.spellSync('colour')) // => false
nodehun.addDictionarySync(dictionaries.en_CA.dictionary)
console.log(nodehun.spellSync('colour')) // => true, no longer incorrect
