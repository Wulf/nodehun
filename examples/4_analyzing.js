const Nodehun = require('bindings')('Nodehun')
const dictionaries = require('./dictionaries')

const nodehun = new Nodehun(dictionaries.en_US.affix, dictionaries.en_US.dictionary)



/**
 * Async : using the promise
 */
nodehun
    .analyze('telling')
    .then(analysis => {
        console.log(analysis) // => [' st:telling ts:0', ' st:tell ts:0 al:told is:Vg']
    })

/**
 * Async : using async/await
 */
async function analyze() {
    const analysis = await nodehun.analyze('telling')
    console.log(analysis) // => [' st:telling ts:0', ' st:tell ts:0 al:told is:Vg']
}
analyze()

/**
 * Sync
 */
const analysis = nodehun.analyzeSync('telling')
console.log(analysis) // => [' st:telling ts:0', ' st:tell ts:0 al:told is:Vg']
