import { fail, equal, throws, ok, deepEqual, notEqual } from "assert"
import { userInfo } from "os"

const Nodehun = require('bindings')('Nodehun')
const fs = require('fs')
const path = require('path')
const enUS = {
    affix: fs.readFileSync(path.resolve(__dirname, './dictionaries/en_us.aff')),
    dictionary: fs.readFileSync(path.resolve(__dirname, './dictionaries/en_us.dic'))
}
const enGB = {
    affix: fs.readFileSync(path.resolve(__dirname, './dictionaries/en_gb.aff')),
    dictionary: fs.readFileSync(path.resolve(__dirname, './dictionaries/en_gb.dic'))
}
const fr = {
    dictionary: fs.readFileSync(path.resolve(__dirname, './dictionaries/fr.dic'))
}
const nl = {
    affix: fs.readFileSync(path.resolve(__dirname, './dictionaries/nl.aff')),
    dictionary: fs.readFileSync(path.resolve(__dirname, './dictionaries/nl.dic'))
}

const Nodehun2 = require('nodehun')
const nodehun2 = new Nodehun2(enUS.affix, enUS.dictionary)
// const nodehun2NL = new Nodehun2(nl.affix, nl.dictionary)

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

    it(`should return false when the word is not spelled correctly`, async () => {
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

    it(`should be a function`, async () => {
        equal(typeof nodehun.spellSync, 'function')
    })

    it(`should throw when 0 arguments are given`, () => {
        try {
            nodehun.spellSync()
            fail()
        } catch {
            // success
        }
    })

    it(`should throw when 2 arguments are given`, () => {
        try {
            nodehun.spellSync(1, 2)
            fail()
        } catch {
            // success
        }
    })

    it(`should throw when the first argument isn't a string`, () => {
        try {
            nodehun.spellSync(123456)
            fail()
        } catch {
            // success
        }
    })

    it(`should return 'false' when a word is not correctly spelled`, () => {
        equal(nodehun.spellSync('colour'), false)
    })
  
    it(`should return 'true' when a word is correctly spelled (1)`, () => {
        equal(nodehun.spellSync('color'), true)
    })
  
    it(`should return 'true' when a word is correctly spelled (2)`, () => {
        equal(nodehun.spellSync('c'), true)
    })
  
    it(`should return 'true' without word`, () => {
        equal(nodehun.spellSync(' '), true)
    })
  
    it(`should return 'true' for non-words`, () => {
        equal(nodehun.spellSync('.'), true)
    })
  
    it(`should check for sentence-case when upper-case (ok)`, () => {
        equal(nodehun.spellSync('ABDUL'), true)
    })
  
    it(`should check for sentence-case when upper-case (not ok)`, () => {
        equal(nodehun.spellSync('COLOUR'), false)
    })
  
    it(`should check for lower-case (ok)`, () => {
        equal(nodehun.spellSync('Color'), true)
    })
  
    it(`should check for lower-case (not ok)`, () => {
        equal(nodehun.spellSync('Colour'), false)
    })
  
    it(`should check for lower-case (not ok)`, () => {
        equal(nodehun.spellSync('Colour'), false)
    })
  
    it(`should not check upper-case for sentence-case when KEEPCASE`, () => {
        equal(nodehunNL.spellSync('DVD'), false)
    })
  
    it(`should not check other casing for lower-case when KEEPCASE`, () => {
        equal(nodehunNL.spellSync('dVd'), false)
    })
  
    it(`should support ONLYINCOMPOUND (ok)`, () => {
        equal(nodehunNL.spellSync('eierlevendbarend'), true)
    })
  
    it(`should support ONLYINCOMPOUND (not ok)`, () => {
        equal(nodehunNL.spellSync('eier'), false)
    })
  
    it(`should support compounds (1)`, () => {
        equal(nodehun.spellSync('21st'), true)
    })
  
    it(`should support compounds (2)`, () => {
        equal(nodehun.spellSync('20st'), false)
    })
  
    it(`should support compounds (3)`, () => {
        equal(nodehun.spellSync('20th'), true)
    })
  
    it(`should support compounds (4)`, () => {
        equal(nodehun.spellSync('23st'), false)
    })
  
    it(`should support compounds (5)`, () => {
        equal(nodehun.spellSync('23th'), false)
    })
  
    it(`should support compounds (6)`, () => {
        equal(nodehun.spellSync('23rd'), true)
    })
  
    it(`should support compounds (7)`, () => {
        equal(nodehun.spellSync('12th'), true)
    })
  
    it(`should support compounds (8)`, () => {
        equal(nodehun.spellSync('22nd'), true)
    })

    it(`should not throw when spellchecking emojis ☀`, () => {
        nodehun.spellSync('😀')
        nodehun.spellSync('☀')
    })
})

describe('Nodehun#suggestSync(word)', () => {
    let nodehun = new Nodehun(enUS.affix, enUS.dictionary)

    it(`should be a function`, () => {
        equal(typeof nodehun.suggestSync, 'function')
    })

    it(`should throw when 0 arguments are given`, () => {
        try {
            nodehun.suggestSync()
            fail()
        } catch {
            // success
        }
    })

    it(`should throw when 2 arguments are given`, () => {
        try {
            nodehun.suggestSync(1, 2)
            fail()
        } catch {
            // success
        }
    })

    it(`should throw when the first argument isn't a string`, () => {
        try {
            nodehun.suggestSync(123456)
            fail()
        } catch {
            // success
        }
    })

    it(`should return null when correct (1)`, () => {
        deepEqual(nodehun.suggestSync('color'), null)

        // TODO: Mention in v2->v3 changelog that suggestions for
        // correctly spelled words return null instead of an
        // empty array.
        //
        // deepEqual(
        //     nodehun3.suggestSync('color'),
        //     nodehun2.spellSuggestionsSync('color')
        // )
    });

    it(`should return null when correct (2)`, () => {
        deepEqual(
            nodehun.suggestSync('c'),
            null
        )
    });

    it(`should suggest alternatives`, () => {
        deepEqual(
            nodehun.suggestSync('colour').slice(0, 5),
            ['color', 'co lour', 'co-lour', 'col our', 'col-our']
        )
    });

    it(`should suggest alternatives`, () => {
        deepEqual(
            nodehun.suggestSync('propper').slice(0, 5),
            ['proper', 'popper', 'prosper', 'cropper', 'propped']
        )
    });

    it(`should return null for empty values`, () => {
        deepEqual(
            nodehun.suggestSync(' '),
            null
        )
    });

    it(`should return null for non-words`, () => {
        deepEqual(
            nodehun.suggestSync('.'),
            null
        )
    });

    it(`should suggest alternatives for sentence-case`, () => {
        deepEqual(
            nodehun.suggestSync('Colour').slice(0, 5),
            ['Co lour', 'Co-lour', 'Col our', 'Col-our', 'Color']
        )
    });

    it(`should suggest alternatives for upper-case`, () => {
        deepEqual(
            nodehun.suggestSync('COLOUR').slice(0, 5),
            ['COLOR', 'CO LOUR', 'CO-LOUR', 'COL OUR', 'COL-OUR']
        )
    });

    it(`should suggest alternatives for funky-case`, () => {
        deepEqual(
            nodehun.suggestSync('coLOUR').slice(0, 5),
            ['col Our', 'co Lour', 'color', 'co-lour', 'col-our']
        )
    });

    it(`should suggest uppercase versions`, () => {
        deepEqual(
            nodehun.suggestSync('html'),
            ['HTML', 'ht ml', 'ht-ml']
        )
    });

    it(`should suggest removals`, () => {
        deepEqual(
            nodehun.suggestSync('collor').slice(0, 5),
            ['color', 'collar', 'coll or', 'coll-or', 'collator']
        )
    });

    it(`should suggest additions`, () => {
        notEqual(
            nodehun.suggestSync('coor').indexOf('color'),
            -1
        )
    });

    it(`should suggest switches`, () => {
        deepEqual(
            nodehun.suggestSync('cloor'),
            ['color', 'floor', 'Clorox', 'clamor', 'Clotho', 'cloy']
        )
    });

    it(`should suggest insertions`, () => {
        deepEqual(
            nodehun.suggestSync('cokor'),
            ['color', 'cork', 'Cork', 'cooker', 'cor', 'cooky', 'Coke']
        )
    });

    it(`should not suggest alternatives marked with 'NOSUGGEST'`, () => {
        deepEqual(
            nodehun.suggestSync('bulshit'),
            ['bullish', 'Bolshevist', 'bolshie']
        )
    });

    it(`should suggest based on replacements`, () => {
        deepEqual(
            nodehun.suggestSync('consize'),
            ['con size', 'con-size', 'consignee', 'consider', 'consign', 'colonize', 'consist', 'concise']
        )
    });

    // TODO: create an issue
    // it(`should not throw when suggesting for emojis ☀`, () => {
    //     nodehun.suggestSync('😀')
    //     nodehun.suggestSync('☀')
    // })

    it(`should not overflow on too long values`, () => {
        const word = 'npmnpmnpmnpmnpmnpmnpmnpmnpmnpmnpmnpmnpmnpmnpm'
        deepEqual(nodehun.suggestSync(word), [])
    });

})

describe('Nodehun#suggest(word)', () => {
    let nodehun: Nodehun = new Nodehun(enUS.affix, enUS.dictionary);

    it(`should be a function`, async () => {
        equal(typeof nodehun.suggest, 'function')
    })

    it(`should return a promise`, async () => {
        let success = false

        await nodehun.suggest()
            .then(() => { })
            .catch(() => { })
            .finally(() => { success = true })
            
        equal(success, true)
    })

    it(`should throw when 0 arguments are given`, async () => {
        try {
            await nodehun.suggest()
            fail()
        } catch {
            // success
        }
    })

    it(`should throw when 2 arguments are given`, async () => {
        try {
            await nodehun.suggest(1, 2)
            fail()
        } catch {
            // success
        }
    })

    it(`should throw when the first argument isn't a string`, async () => {
        try {
            await nodehun.suggest(123456)
            fail()
        } catch {
            // success
        }
    })

    it(`should return null when the word is spelled correctly`, async () => {
        equal(await nodehun.suggest('color'), null)
    })

    it(`should return an array when the word is not spelled correctly`, async () => {
        const value = await nodehun.suggest('colour')
        equal(typeof value, 'object')
        equal(typeof value.length, 'number')
    })
    
    it(`should return appropriate suggestions when a word is spelled incorrectly`, async () => {
        const value = await nodehun.suggest('colour')
        deepEqual(value.splice(0, 3), ['color', 'co lour', 'co-lour'])
    })

    // TODO: create an issue
    // it(`should not throw when suggesting for emojis ☀`, async () => {
    //     await nodehun.suggest('😀')
    //     await nodehun.suggest('☀')
    // })
})

describe('Nodehun#add(word)', () => {
    let nodehun: Nodehun

    beforeEach(() => {
        // clear changes before each test
        nodehun = new Nodehun(enUS.affix, enUS.dictionary)
    })
        
    it(`should be a function`, async () => {
        equal(typeof nodehun.add, 'function')
    })

    it(`should return a promise`, async () => {
        let success = false

        await nodehun.add()
            .then(() => { })
            .catch(() => { })
            .finally(() => { success = true })
            
        equal(success, true)
    })

    it(`should throw when 0 arguments are given`, async () => {
        try {
            await nodehun.add()
            fail()
        } catch {
            // success
        }
    })

    it(`should throw when 2 arguments are given`, async () => {
        try {
            await nodehun.add(1, 2)
            fail()
        } catch {
            // success
        }
    })

    it(`should throw when the first argument isn't a string`, async () => {
        try {
            await nodehun.add(123456)
            fail()
        } catch {
            // success
        }
    })

    it(`should now mark as correct`, async () => {
        const word = 'npm'
        equal(await nodehun.spell(word), false)
        await nodehun.add('npm')
        equal(await nodehun.spell(word), true)
    })

    it(`should no longer receive suggestions`, async () => {
        const word = 'npm'

        notEqual(await nodehun.suggest(word), null)
        await nodehun.add(word)
        equal(await nodehun.suggest(word), null)
    })
})

describe('Nodehun#addSync(value)', () => {
    let nodehun: Nodehun

    beforeEach(() => {
        // clear changes before each test
        nodehun = new Nodehun(enUS.affix, enUS.dictionary)
    })

    it(`should be a function`, () => {
        equal(typeof nodehun.addSync, 'function')
    })

    it(`should throw when 0 arguments are given`, () => {
        try {
            nodehun.addSync()
            fail()
        } catch {
            // success
        }
    })

    it(`should throw when 2 arguments are given`, () => {
        try {
            nodehun.addSync(1, 2)
            fail()
        } catch {
            // success
        }
    })

    it(`should throw when the first argument isn't a string`, () => {
        try {
            nodehun.addSync(123456)
            fail()
        } catch {
            // success
        }
    })

    it(`should now mark as correct`, () => {
        const word = 'npm'

        equal(nodehun.spellSync(word), false)
        nodehun.addSync(word)
        equal(nodehun.spellSync(word), true)
    })

    it(`should no longer receive suggestions`, () => {
        const word = 'npm'

        notEqual(nodehun.suggestSync(word), null)
        nodehun.addSync(word)
        equal(nodehun.suggestSync(word), null)
    })
})

describe('Nodehun#remove(word)', () => {
    let nodehun: Nodehun

    beforeEach(() => {
        // clear changes before each test
        nodehun = new Nodehun(enUS.affix, enUS.dictionary)
    })
        
    it(`should be a function`, async () => {
        equal(typeof nodehun.remove, 'function')
    })

    it(`should return a promise`, async () => {
        let success = false

        await nodehun.remove()
            .then(() => { })
            .catch(() => { })
            .finally(() => { success = true })
            
        equal(success, true)
    })

    it(`should throw when 0 arguments are given`, async () => {
        try {
            await nodehun.remove()
            fail()
        } catch {
            // success
        }
    })

    it(`should throw when 2 arguments are given`, async () => {
        try {
            await nodehun.remove(1, 2)
            fail()
        } catch {
            // success
        }
    })

    it(`should throw when the first argument isn't a string`, async () => {
        try {
            await nodehun.remove(123456)
            fail()
        } catch {
            // success
        }
    })

    it(`should now mark as correct`, async () => {
        const word = 'npm'

        await nodehun.add(word)
        equal(await nodehun.spell(word), true)
        await nodehun.remove('npm')
        equal(await nodehun.spell(word), false)
    })

    it(`should no longer receive suggestions`, async () => {
        const word = 'npm'

        await nodehun.add(word)
        equal(await nodehun.suggest(word), null)
        await nodehun.remove(word)
        notEqual(await nodehun.suggest(word), null)
    })
})

describe('Nodehun#removeSync(value)', () => {
    let nodehun: Nodehun

    beforeEach(() => {
        // clear changes before each test
        nodehun = new Nodehun(enUS.affix, enUS.dictionary)
    })

    it(`should be a function`, () => {
        equal(typeof nodehun.removeSync, 'function')
    })

    it(`should throw when 0 arguments are given`, () => {
        try {
            nodehun.removeSync()
            fail()
        } catch {
            // success
        }
    })

    it(`should throw when 2 arguments are given`, () => {
        try {
            nodehun.removeSync(1, 2)
            fail()
        } catch {
            // success
        }
    })

    it(`should throw when the first argument isn't a string`, () => {
        try {
            nodehun.removeSync(123456)
            fail()
        } catch {
            // success
        }
    })

    it(`should now mark as correct`, () => {
        const word = 'npm'

        nodehun.addSync(word)
        equal(nodehun.spellSync(word), true)
        nodehun.removeSync(word)
        equal(nodehun.spellSync(word), false)
    })

    it(`should no longer receive suggestions`, () => {
        const word = 'npm'

        nodehun.addSync(word)
        equal(nodehun.suggestSync(word), null)
        nodehun.removeSync(word)
        notEqual(nodehun.suggestSync(word), null)
    })
})

  // t.test('Nodehun#addSync(value, model)', function (st) {
  //   /* `azc` is a Dutch word only properly spelled
  //    * in its lower-case form. */
  //   st.equal(
  //     nl.addSync('npm', 'azc'),
  //     nl,
  //     'should return the context object'
  //   );
  //
  //   st.equal(nl.spellSync('npm'), true, 'should match affixes (1)');
  //   st.equal(nl.spellSync('NPM'), false, 'should match affixes (2)');
  //   st.equal(nl.spellSync('Npm'), false, 'should match affixes (3)');
  //
  //   nl.removeSync('npm');
  //
  //   st.end();
  // });


// TODO:
// `stem`, `generate`, and `analyze`.

describe('Nodehun#analyze(word: string): Promise<string[]>;', () => {
    const nodehun: Nodehun = new Nodehun(enUS.affix, enUS.dictionary)

    it(`should be a function`, () => {
        equal(typeof nodehun.analyze, 'function')
    })

    it(`should return a promise`, async () => {
        let success = false
        await nodehun.analyze()
            .then(() => {})
            .catch(() => {})
            .finally(() => { success = true })

        equal(success, true)
    })

    it(`should throw when no arguments are given`, async () => {
        try {
            await nodehun.analyze()
            fail()
        } catch {
            // success
        }
    })

    it(`should throw when 2 arguments are given`, async () => {
        try {
            await nodehun.analyze(1, 2)
            fail()
        } catch {
            // success
        }
    })

    it(`should throw when the first argument isn't a string`, async () => {
        try {
            await nodehun.analyze(1)
            fail()
        } catch {
            // success
        }
    })
    
    it(`should return morphological analysis`, async () => {
        const morphologicalAnalysis = await nodehun.analyze('telling')
        deepEqual(
            morphologicalAnalysis,
            [' st:telling ts:0', ' st:tell ts:0 al:told is:Vg']
        )
    })

    it(`should return an empty array when it isn't available`, async () => {
        deepEqual(
            await nodehun.analyze('npmnpmnpmnpmnpmnpmnpmnpm'),
            []
        )
    })
})

describe('Nodehun#analyzeSync(word: string): string[];', () => {
    const nodehun: Nodehun = new Nodehun(enUS.affix, enUS.dictionary)

    it(`should be a function`, () => {
        equal(typeof nodehun.analyzeSync, 'function')
    })

    it(`should throw when no arguments are given`, () => {
        try {
            nodehun.analyzeSync()
            fail()
        } catch {
            // success
        }
    })

    it(`should throw when 2 arguments are given`, () => {
        try {
            nodehun.analyzeSync(1, 2)
            fail()
        } catch {
            // success
        }
    })

    it(`should throw when the first argument isn't a string`, () => {
        try {
            nodehun.analyzeSync(1)
            fail()
        } catch {
            // success
        }
    })
    
    it(`should return morphological analysis`, async () => {
        const morphologicalAnalysis = nodehun.analyzeSync('telling')
        deepEqual(
            morphologicalAnalysis,
            [' st:telling ts:0', ' st:tell ts:0 al:told is:Vg']
        )
    })

    it(`should return an empty array when it isn't available`, async () => {
        deepEqual(
            nodehun.analyzeSync('npmnpmnpmnpmnpmnpmnpmnpm'),
            []
        )
    })
})

describe('Nodehun#stem(word: string): Promise<string[]>;', () => {
    const nodehun: Nodehun = new Nodehun(enUS.affix, enUS.dictionary)

    it(`should be a function`, () => {
        equal(typeof nodehun.stem, 'function')
    })

    it(`should return a promise`, async () => {
        let success = false
        await nodehun.stem()
            .then(() => {})
            .catch(() => {})
            .finally(() => { success = true })

        equal(success, true)
    })

    it(`should throw when no arguments are given`, async () => {
        try {
            await nodehun.stem()
            fail()
        } catch {
            // success
        }
    })

    it(`should throw when 2 arguments are given`, async () => {
        try {
            await nodehun.stem(1, 2)
            fail()
        } catch {
            // success
        }
    })

    it(`should throw when the first argument isn't a string`, async () => {
        try {
            await nodehun.stem(1)
            fail()
        } catch {
            // success
        }
    })
    
    it(`should return roots`, async () => {
        const roots = await nodehun.stem('telling')
        deepEqual(
            roots,
            ['telling', 'tell']
        )
    })

    it(`should return an empty array when not available`, async () => {
        deepEqual(
            await nodehun.stem('npmnpmnpmnpmnpmnpmnpmnpm'),
            []
        )
    })
})

describe('Nodehun#stemSync(word: string): string[];', () => {
    const nodehun: Nodehun = new Nodehun(enUS.affix, enUS.dictionary)

    it(`should be a function`, () => {
        equal(typeof nodehun.stemSync, 'function')
    })

    it(`should throw when no arguments are given`, () => {
        try {
            nodehun.stemSync()
            fail()
        } catch {
            // success
        }
    })

    it(`should throw when 2 arguments are given`, () => {
        try {
            nodehun.stemSync(1, 2)
            fail()
        } catch {
            // success
        }
    })

    it(`should throw when the first argument isn't a string`, () => {
        try {
            nodehun.stemSync(1)
            fail()
        } catch {
            // success
        }
    })
    
    it(`should return roots`, async () => {
        const roots = nodehun.stemSync('telling')
        deepEqual(
            roots,
            ['telling', 'tell']
        )
    })

    it(`should return an empty array when not available`, async () => {
        deepEqual(
            nodehun.stemSync('npmnpmnpmnpmnpmnpmnpmnpm'),
            []
        )
    })
})

describe('Nodehun#generate(word: string, example: string): Promise<string[]>;', () => {
    const nodehun: Nodehun = new Nodehun(enUS.affix, enUS.dictionary)

    it(`should be a function`, () => {
        equal(typeof nodehun.generate, 'function')
    })

    it(`should return a promise`, async () => {
        let success = false
        await nodehun.generate()
            .then(() => {})
            .catch(() => {})
            .finally(() => { success = true })

        equal(success, true)
    })

    it(`should throw when no arguments are given`, async () => {
        try {
            await nodehun.generate()
            fail()
        } catch {
            // success
        }
    })

    it(`should throw when 3 arguments are given`, async () => {
        try {
            await nodehun.generate(1, 2, 3)
            fail()
        } catch {
            // success
        }
    })

    it(`should throw when the first argument isn't a string`, async () => {
        try {
            await nodehun.generate(1)
            fail()
        } catch {
            // success
        }
    })

    it(`should throw when the second argument isn't a string`, async () => {
        try {
            await nodehun.generate('abc', 1)
            fail()
        } catch {
            // success
        }
    })

    
    it(`should return variations based on example`, async () => {
        deepEqual(
            await nodehun.generate('telling', 'ran'),
            [ 'told' ]
        )
    })

    it(`should return variations based on example (2)`, async () => {
        deepEqual(
            await nodehun.generate('told', 'run'),
            [ 'tell' ]
        )
    })

    it(`should return an empty array when not computable`, async () => {
        deepEqual(
            await nodehun.generate('told', 'npmnpmnpmnpm'),
            []
        )
    })

    it(`should return an empty array when not computable (2)`, async () => {
        deepEqual(
            await nodehun.generate('npmnpmnpmnpm', 'npmnpmnpmnpm'),
            []
        )
    })

    it(`should return an empty array when not computable (3)`, async () => {
        deepEqual(
            await nodehun.generate('npmnpmnpmnpm', 'run'),
            []
        )
    })
})

describe('Nodehun#generateSync(word: string): string[];', () => {
    const nodehun: Nodehun = new Nodehun(enUS.affix, enUS.dictionary)

    it(`should be a function`, () => {
        equal(typeof nodehun.generateSync, 'function')
    })

    it(`should throw when no arguments are given`, () => {
        try {
            nodehun.generateSync()
            fail()
        } catch {
            // success
        }
    })

    it(`should throw when 3 arguments are given`, async () => {
        try {
            nodehun.generateSync(1, 2, 3)
            fail()
        } catch {
            // success
        }
    })

    it(`should throw when the first argument isn't a string`, async () => {
        try {
            nodehun.generateSync(1)
            fail()
        } catch {
            // success
        }
    })

    it(`should throw when the second argument isn't a string`, async () => {
        try {
            nodehun.generateSync('abc', 1)
            fail()
        } catch {
            // success
        }
    })

    
    it(`should return variations based on example`, async () => {
        deepEqual(
            nodehun.generateSync('telling', 'ran'),
            [ 'told' ]
        )
    })

    it(`should return variations based on example (2)`, async () => {
        deepEqual(
            nodehun.generateSync('told', 'run'),
            [ 'tell' ]
        )
    })

    it(`should return an empty array when not computable`, async () => {
        deepEqual(
            nodehun.generateSync('told', 'npmnpmnpmnpm'),
            []
        )
    })

    it(`should return an empty array when not computable (2)`, async () => {
        deepEqual(
            nodehun.generateSync('npmnpmnpmnpm', 'npmnpmnpmnpm'),
            []
        )
    })

    it(`should return an empty array when not computable (3)`, async () => {
        deepEqual(
            nodehun.generateSync('npmnpmnpmnpm', 'run'),
            []
        )
    })
})

describe('Nodehun#addDictionary(dictionary: Buffer): Promise<void>;', () => {
    const nodehun: Nodehun = new Nodehun(enUS.affix, enUS.dictionary)

    it(`should be a function`, () => {
        equal(typeof nodehun.addDictionary, 'function')
    })

    it(`should return a promise`, async () => {
        let success = false
        await nodehun.addDictionary()
            .then(() => {})
            .catch(() => {})
            .finally(() => { success = true })

        equal(success, true)
    })

    it(`should throw when no arguments are given`, async () => {
        try {
            await nodehun.addDictionary()
            fail()
        } catch {
            // success
        }
    })

    it(`should throw when 2 arguments are given`, async () => {
        try {
            await nodehun.addDictionary(1, 2)
            fail()
        } catch {
            // success
        }
    })

    it(`should throw when the first argument isn't a string`, async () => {
        try {
            await nodehun.addDictionary(1)
            fail()
        } catch {
            // success
        }
    })
    
    it(`should mark correct after dictionary is added`, async () => {
        await nodehun.addDictionary(fr.dictionary)
        equal(await nodehun.spell('bonjour'), true)
    })
})

describe('Nodehun#addDictionarySync(dictionary: Buffer): void;', () => {
    const nodehun: Nodehun = new Nodehun(enUS.affix, enUS.dictionary)

    it(`should be a function`, () => {
        equal(typeof nodehun.addDictionarySync, 'function')
    })

    it(`should throw when no arguments are given`, () => {
        try {
            nodehun.addDictionarySync()
            fail()
        } catch {
            // success
        }
    })

    it(`should throw when 2 arguments are given`, () => {
        try {
            nodehun.addDictionarySync(1, 2)
            fail()
        } catch {
            // success
        }
    })

    it(`should throw when the first argument isn't a string`, () => {
        try {
            nodehun.addDictionarySync(1)
            fail()
        } catch {
            // success
        }
    })
    
    it(`should mark correct after dictionary is added`, async () => {
        nodehun.addDictionarySync(fr.dictionary)
        equal(nodehun.spellSync('bonjour'), true)
    })
})

describe('Nodehun#addWithAffix(word: string, example: string): Promise<void>;', () => {
    let nodehun: Nodehun
    
    beforeEach(() => {
        // clear changes before every test
        nodehun = new Nodehun(enUS.affix, enUS.dictionary)
     })

    it(`should be a function`, () => {
        equal(typeof nodehun.addWithAffix, 'function')
    })

    it(`should return a promise`, async () => {
        let success = false
        await nodehun.addWithAffix()
            .then(() => {})
            .catch(() => {})
            .finally(() => { success = true })

        equal(success, true)
    })

    it(`should throw when no arguments are given`, async () => {
        try {
            await nodehun.addWithAffix()
            fail()
        } catch {
            // success
        }
    })

    it(`should throw when 3 arguments are given`, async () => {
        try {
            await nodehun.addWithAffix(1, 2, 3)
            fail()
        } catch {
            // success
        }
    })

    it(`should throw when the first argument isn't a string`, async () => {
        try {
            await nodehun.addWithAffix(1)
            fail()
        } catch {
            // success
        }
    })

    it(`should throw when the second argument isn't a string`, async () => {
        try {
            await nodehun.addWithAffix('abc', 1)
            fail()
        } catch {
            // success
        }
    })

    it(`should mark correct`, async () => {
        await nodehun.addWithAffix('colour', 'color')
        equal(await nodehun.spell('colouring'), true)
    })
})

describe('Nodehun#addWithAffixSync(word: string, example: string): void;', () => {
    const nodehun: Nodehun = new Nodehun(enUS.affix, enUS.dictionary)

    it(`should be a function`, () => {
        equal(typeof nodehun.addWithAffixSync, 'function')
    })

    it(`should throw when no arguments are given`, () => {
        try {
            nodehun.addWithAffixSync()
            fail()
        } catch {
            // success
        }
    })

    it(`should throw when 3 arguments are given`, () => {
        try {
            nodehun.addWithAffixSync(1, 2,3)
            fail()
        } catch {
            // success
        }
    })

    it(`should throw when the first argument isn't a string`, () => {
        try {
            nodehun.addWithAffixSync(1)
            fail()
        } catch {
            // success
        }
    })

    it(`should throw when the second argument isn't a string`, () => {
        try {
            nodehun.addWithAffixSync('abc', 2)
            fail()
        } catch {
            // success
        }
    })

    it(`should mark correct`, async () => {
        nodehun.addWithAffixSync('colour', 'color')
        equal(nodehun.spellSync('colouring'), true)
    })
})

describe('Nodehun#getWordCharacters()', () => {
    const nodehun: Nodehun = new Nodehun(enUS.affix, enUS.dictionary);
    const nodehunGB: Nodehun = new Nodehun(enGB.affix, enGB.dictionary);

    it(`should return the defined word-characters`, () => {
        equal(nodehun.getWordCharacters(), `0123456789'.-’`)
    })
    
    // TODO: change all `equal` tests to `strictEqual`

    it(`should return 'undefined' when not defined`, () => {
        equal(nodehunGB.getWordCharacters(), undefined)
    })
});

describe('Nodehun#getWordCharactersUTF16()', () => {
    const nodehun: Nodehun = new Nodehun(enUS.affix, enUS.dictionary);
    const nodehunGB: Nodehun = new Nodehun(enGB.affix, enGB.dictionary);

    it(`should return the defined word-characters`, () => {
        equal(nodehun.getWordCharactersUTF16(), `'-.0123456789’`)
    })
  
    it(`should return 'undefined' when not defined`, () => {
        equal(nodehunGB.getWordCharactersUTF16(), undefined)
    })
});

describe('Nodehun#getDictionaryEncoding()', () => {
    const nodehun: Nodehun = new Nodehun(enUS.affix, enUS.dictionary);

    it(`should return encoding as a string when known`, () => {
        equal(nodehun.getDictionaryEncoding(), 'UTF-8')
    })
})

describe('Nodehun#getVersion()', () => {
    const nodehun: Nodehun = new Nodehun(enUS.affix, enUS.dictionary);

    it(`should return 'undefined' when not defined`, () => {
        equal(nodehun.getVersion(), undefined)
    })
})
