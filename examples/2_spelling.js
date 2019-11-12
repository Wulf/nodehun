const Nodehun = require('bindings')('Nodehun')
const dictionaries = require('./dictionaries')

const nodehun = new Nodehun(dictionaries.en_US.affix, dictionaries.en_US.dictionary)



/**
 * Async : using the promise
 */
nodehun
    .spell('color')
    .then(isCorrect => {
        console.log('"color" is correct?', isCorrect) // => true
    })

/**
 * Async : using async/await
 */
async function spell() {
    const isCorrect = await nodehun.spell('color')
    console.log('"color" is correct?', isCorrect) // => true
}
spell()

/**
 * Sync
 */
const isCorrect = nodehun.spellSync('colour')
console.log('"colour" is correct?', isCorrect) // => false
