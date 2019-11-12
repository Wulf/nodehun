const Nodehun = require('bindings')('Nodehun')
const dictionaries = require('./dictionaries')

const nodehun = new Nodehun(dictionaries.en_US.affix, dictionaries.en_US.dictionary)



/**
 * Async : using the promise
 */
nodehun
    .suggest('color')
    .then(suggestions => {
        console.log(suggestions) // => null because 'color' is correct
    })

/**
 * Async : using async/await
 */
async function suggest() {
    const suggestions = await nodehun.suggest('color')
    console.log(suggestions) // => null because 'color' is correct
}
suggest()

/**
 * Sync
 */
const suggestions = nodehun.suggestSync('colour')
console.log(suggestions) // => array of suggestions
