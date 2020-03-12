const Nodehun = require('bindings')('Nodehun')
const dictionaries = require('./dictionaries')

const nodehun = new Nodehun(dictionaries.en_US.affix, dictionaries.en_US.dictionary)



/**
 * Async : using the promise
 */
nodehun
    .stem('telling')
    .then(stems => {
        console.log(stems) // => ['telling', 'tell']
    })

/**
 * Async : using async/await
 */
async function getStems() {
    const stems = await nodehun.stem('telling')
    console.log(stems) // => ['telling', 'tell']
}
getStems()

/**
 * Sync
 */
const stems = nodehun.stemSync('telling')
console.log(stems) // => ['telling', 'tell']
