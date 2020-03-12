const Nodehun = require('bindings')('Nodehun')
const dictionaries = require('./dictionaries')

const nodehun = new Nodehun(dictionaries.en_US.affix, dictionaries.en_US.dictionary)



/**
 * Async : using the promise
 */
nodehun
    .generate('telling', 'ran')
    .then(generate => {
        console.log(generate) // => ['told']
    })

/**
 * Async : using async/await
 */
async function generateExample() {
    const generate = await nodehun.generate('telling', 'ran')
    console.log(generate) // => ['told']
}
generateExample()

/**
 * Sync
 */
const generate = nodehun.generateSync('told', 'run')
console.log(generate) // => ['tell']
