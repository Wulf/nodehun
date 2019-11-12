const Nodehun = require('bindings')('Nodehun')
const dictionaries = require('./dictionaries')

let nodehun

/**
 * Async : using the promise
 */
nodehun = new Nodehun(dictionaries.en_US.affix, dictionaries.en_US.dictionary)
console.log(nodehun.spellSync('colour')) // => false
nodehun
    .add('colour')
    .then(() => {
        console.log(nodehun.spellSync('colour')) // => true, no longer incorrect
    })

/**
 * Async : using async/await
 */
nodehun = new Nodehun(dictionaries.en_US.affix, dictionaries.en_US.dictionary)
async function generate() {
    console.log(nodehun.spellSync('colour')) // => false
    await nodehun.add('colour')
    console.log(nodehun.spellSync('colour')) // => true, no longer incorrect
}
generate()

/**
 * Sync
 */
nodehun = new Nodehun(dictionaries.en_US.affix, dictionaries.en_US.dictionary)
console.log(nodehun.spellSync('colour')) // => false
nodehun.addSync('colour')
console.log(nodehun.spellSync('colour')) // => true, no longer incorrect
