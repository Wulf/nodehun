const Nodehun = require('bindings')('Nodehun')
const dictionaries = require('./dictionaries')

let nodehun

/**
 * Async : using the promise
 */
nodehun = new Nodehun(dictionaries.en_US.affix, dictionaries.en_US.dictionary)
nodehun.addSync('colour')
console.log(nodehun.spellSync('colour')) // => true
nodehun
    .remove('colour')
    .then(() => {
        console.log(nodehun.spellSync('colour')) // => false, no longer correct
    })

/**
 * Async : using async/await
 */
nodehun = new Nodehun(dictionaries.en_US.affix, dictionaries.en_US.dictionary)
nodehun.addSync('colour')
async function generate() {
    console.log(nodehun.spellSync('colour')) // => true
    await nodehun.remove('colour')
    console.log(nodehun.spellSync('colour')) // => false, no longer correct
}
generate()

/**
 * Sync
 */
nodehun = new Nodehun(dictionaries.en_US.affix, dictionaries.en_US.dictionary)
nodehun.addSync('colour')
console.log(nodehun.spellSync('colour')) // => true
nodehun.removeSync('colour')
console.log(nodehun.spellSync('colour')) // => false, no longer correct
