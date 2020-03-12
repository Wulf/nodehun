const Nodehun = require('bindings')('Nodehun')
const dictionaries = require('./dictionaries')

let nodehun

/**
 * Async : using the promise
 */
nodehun = new Nodehun(dictionaries.en_US.affix, dictionaries.en_US.dictionary)
console.log(nodehun.spellSync('colouring')) // => false
nodehun
    .addWithAffix('colour', 'color')
    .then(() => {
        console.log(nodehun.spellSync('colouring')) // => true, no longer incorrect
    })

/**
 * Async : using async/await
 */
nodehun = new Nodehun(dictionaries.en_US.affix, dictionaries.en_US.dictionary)
async function generate() {
    console.log(nodehun.spellSync('colouring')) // => false
    await nodehun.addWithAffix('colour', 'color')
    console.log(nodehun.spellSync('colouring')) // => true, no longer incorrect
}
generate()

/**
 * Sync
 */
nodehun = new Nodehun(dictionaries.en_US.affix, dictionaries.en_US.dictionary)
console.log(nodehun.spellSync('colouring')) // => false
nodehun.addWithAffixSync('colour', 'color')
console.log(nodehun.spellSync('colouring')) // => true, no longer incorrect
