import { fail, equal, throws, ok } from "assert"

const Nodehun = require('bindings')('Nodehun')
const fs = require('fs')
const path = require('path')
const enUS = {
    affix: fs.readFileSync(path.resolve(__dirname, './en_us.aff')),
    dictionary: fs.readFileSync(path.resolve(__dirname, './en_us.dic'))
}
const fr = {
    dictionary: fs.readFileSync(path.resolve(__dirname, './fr.dic'))
}
const nl = {
    affix: fs.readFileSync(path.resolve(__dirname, './nl.aff')),
    dictionary: fs.readFileSync(path.resolve(__dirname, './nl.dic'))
}

describe('Nodehun(affixBuffer, dictionaryBuffer)', () => {
    it(`should export a function`, () => {
        equal(typeof Nodehun, 'function')
    })

    it(`should throw when 'new' operator isn't used`, () => {
        throws(() => Nodehun())
    })

    it(`should throw when no arguments are given`, () => {
        throws(() => new Nodehun())
    })

    it(`should throw when 1 arguments are given`, () => {
        throws(() => new Nodehun(1))
    })

    it(`should throw when 3 arguments are given`, () => {
        throws(() => new Nodehun(1, 2, 3))
    })

    it(`should throw when the first argument isn't a buffer`, () => {
        throws(() => new Nodehun(1, 2))
    })

    it(`should throw when the second argument isn't a buffer`, () => {
        throws(() => new Nodehun(enUS.affix, 2))
    })

    it(`should successfully construct an object when two buffers are given`, () => {
        new Nodehun(enUS.affix, enUS.dictionary)
    })

    // FIXME

    // it(`should construct an object of type Nodehun`, () => {
    //     const nodehun = new Nodehun(enUS.affix, enUS.dictionary)

    //     equal(nodehun instanceof Nodehun, true)
    // })
})

describe('Nodehun#spell(word)', () => {
    let nodehun: Nodehun = new Nodehun(enUS.affix, enUS.dictionary);

    it(`should be a function`, async () => {
        equal(typeof nodehun.spell, 'function')
    })

    it(`should return a promise`, async () => {
        let success = false

        await nodehun.spell()
            .then(() => { })
            .catch(() => { })
            .finally(() => { success = true })
        
        equal(success, true)
    })

    it(`should throw when 0 arguments are given`, async () => {
        try {
            await nodehun.spell()
            fail()
        } catch {
            // success
        }
    })

    it(`should throw when 2 arguments are given`, async () => {
        try {
            await nodehun.spell(1, 2)
            fail()
        } catch {
            // success
        }
    })

    it(`should throw when the first argument isn't a string`, async () => {
        try {
            await nodehun.spell(123456)
            fail()
        } catch {
            // success
        }
    })

    it(`should return true when the word is spelled correctly`, async () => {
        equal(await nodehun.spell('color'), true)
    })

    it(`should return false when the word is spelled correctly`, async () => {
        equal(await nodehun.spell('colour'), false)
    })

    it(`should not throw when spellchecking emojis ☀`, async () => {
        await nodehun.spell('😀')
        await nodehun.spell('☀')
    })
})

describe('Nodehun#spellSync(word)', () => {
    let nodehun = new Nodehun(enUS.affix, enUS.dictionary)
    let nodehunNL = new Nodehun(nl.affix, nl.dictionary)

    async function test() {
        let ttt = 1000
        let tt = 0
        for (let i = 0; i < ttt; i++) {
            const t = await nodehun.spell('eierlevendbarend')
            if(t) tt++
        }
        if (tt > 0) console.log('>>> ', tt, '/', ttt)
    }

    it(`should be a function`, async () => {
        equal(typeof nodehun.spellSync, 'function')
        await test()
    })

    it(`should return 'false' when a word is not correctly spelled`, async () => {
        equal(nodehun.spellSync('colour'), false)
        await test()
    })
  
    it(`should return 'true' when a word is correctly spelled (1)`, async () => {
        equal(nodehun.spellSync('color'), true)
        await test()
    })
  
    it(`should return 'true' when a word is correctly spelled (2)`, async () => {
        equal(nodehun.spellSync('c'), true)
        await test()
    })
  
    it(`should return 'true' without word`, async () => {
        equal(nodehun.spellSync(' '), true)
        await test()
    })
  
    it(`should return 'true' for non-words`, async () => {
        equal(nodehun.spellSync('.'), true)
        await test()
    })
  
    it(`should check for sentence-case when upper-case (ok)`, async () => {
        equal(nodehun.spellSync('ABDUL'), true)
        await test()
    })
  
    it(`should check for sentence-case when upper-case (not ok)`, async () => {
        equal(nodehun.spellSync('COLOUR'), false)
        await test()
    })
  
    it(`should check for lower-case (ok)`, async () => {
        equal(nodehun.spellSync('Color'), true)
        await test()
    })
  
    it(`should check for lower-case (not ok)`, async () => {
        equal(nodehun.spellSync('Colour'), false)
        await test()
    })
  
    it(`should check for lower-case (not ok)`, async () => {
        equal(nodehun.spellSync('Colour'), false)
        await test()
    })
  
    it(`should not check upper-case for sentence-case when KEEPCASE`, async () => {
        equal(nodehunNL.spellSync('DVD'), false)
        await test()
    })
  
    it(`should not check other casing for lower-case when KEEPCASE`, async () => {
        equal(nodehunNL.spellSync('dVd'), false)
        await test()
    })
  
    // it(`should support ONLYINCOMPOUND (ok)`, async () => {
    //     equal(nodehunNL.spellSync('eierlevendbarend'), true)
    //     await test()
    // })
  
    it(`should support ONLYINCOMPOUND (not ok)`, async () => {
        equal(nodehunNL.spellSync('eier'), false)
        await test()
    })
  
    it(`should support compounds (1)`, async () => {
        equal(nodehun.spellSync('21st'), true)
        await test()
    })
  
    it(`should support compounds (2)`, async () => {
        equal(nodehun.spellSync('20st'), false)
        await test()
    })
  
    it(`should support compounds (3)`, async () => {
        equal(nodehun.spellSync('20th'), true)
        await test()
    })
  
    it(`should support compounds (4)`, async () => {
        equal(nodehun.spellSync('23st'), false)
        await test()
    })
  
    it(`should support compounds (5)`, async () => {
        equal(nodehun.spellSync('23th'), false)
        await test()
    })
  
    it(`should support compounds (6)`, async () => {
        equal(nodehun.spellSync('23rd'), true)
        await test()
    })
  
    it(`should support compounds (7)`, async () => {
        equal(nodehun.spellSync('12th'), true)
        await test()
    })
  
    it(`should support compounds (8)`, async () => {
        equal(nodehun.spellSync('22nd'), true)
        await test()
    })
})

// async function test() {
//     try {
//         var nodehun = new Nodehun(enUS.affix, enUS.dictionary)
//         // console.log(await nodehun.spell('colour'), 'spell (incorrect)')
//         // console.log(await nodehun.spell('color'), 'spell (correct)')
//         // console.log(await nodehun.spell('bonjour'), await nodehun.addDictionary(fr.dictionary), await nodehun.spell('bonjour'), 'addDictionary')
//         // console.log(await nodehun.suggest('calor'), 'suggest')
//         // console.log(await nodehun.analyze('telling'), 'analyze')
//         // console.log(await nodehun.stem('telling'), 'stem')
//         // console.log(await nodehun.generate('telling', 'ran'), 'generate')
//         // console.log(await nodehun.add('asdfff'), await nodehun.spell('asdfff'), 'add')
//         // console.log(await nodehun.addWithAffix('colour', 'color'), await nodehun.spell('colouring'), 'addWithAffix')
//         // console.log(await nodehun.remove('asdfff'), await nodehun.spell('asdfff'), 'remove')
//         // console.log(nodehun.getDictionaryEncoding(), '\tgetDictionaryEncoding')
//         // console.log(nodehun.getWordCharacters(), '\tgetWordCharacters')
//         // console.log(nodehun.getWordCharactersUTF16(), '\tgetWordCharactersUTF16')
//         // console.log(nodehun.getVersion(), 'getVersion')
//         console.log()
//         console.log('>>>>>>>>>>>>>>>>>>>>', await nodehun.spell(1234))
//     } catch (e) {
//         console.log(">>>>>>>>>>>>>>>> Caught exception\n", e)
//     }
// }
// setTimeout(test, 1000)

