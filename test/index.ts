const test = require('tape');
const Nodehun = require('bindings')('Nodehun');

/* Constants. */
const EN_GB = 'en-gb';
const EN_US = 'en-us';
const NL = 'nl';

/**
 * Start the tests with loaded `dictionaries`.
 *
 * @param {Object} dictionaries - Loaded dictionaries.
 */
function start(dictionaries) {
  test('nodehun()', function (t) {
    let us: Nodehun;
    let gb: Nodehun;
    let nl: Nodehun;

    t.equal(
      typeof Nodehun,
      'function',
      'should expose a function'
    );

    t.throws(
      function () {
        Nodehun();
      },
      /^Error: Use the new operator to create an instance of this object\.$/,
      'should throw without `new`'
    );

    t.throws(
      function () {
        // @ts-ignore: this is done on purpose!
        new Nodehun();
      },
      /^Error: Invalid number of arguments\.$/,
      'should throw when missing parameters'
    );

    us = new Nodehun(dictionaries[EN_US].aff, dictionaries[EN_US].dic);
    gb = new Nodehun(dictionaries[EN_GB].aff, dictionaries[EN_GB].dic);
    nl = new Nodehun(dictionaries[NL].aff, dictionaries[NL].dic);

    t.equal(
      us instanceof Nodehun,
      true,
      'should create a new instance from a dictionary (1)'
    );

    t.equal(
      gb instanceof Nodehun,
      true,
      'should create a new instance from a dictionary (2)'
    );

    t.test('Nodehun#getWordCharacters()', function (st) {
      st.equal(
        us.getWordCharacters(),
        '0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖØÙÚÛÜÝÞàáâãäåæçèéêëìíîïðñòóôõöøùúûüýþ',
        'should return the defined word-characters'
      );
    
      st.equal(
        gb.getWordCharacters(),
        null,
        'should return `null` when not defined'
      );
    
      st.end();
    });

    t.test('Nodehun#spell(value)', async function (st) {
      try {
        const spell = await us.spell('colour');
        st.equal(spell, false, 'should pass `false` when a word is not correctly spelled')
      } catch (e) {
        st.fail(e, 'should not throw');
      }
      
      try {
        const spell = await gb.spell('colour');
        st.equal(spell, true, 'should pass `true` when a word is correctly spelled')
      } catch (e) {
        st.fail(e, 'should not throw');
      }

      try {
        const spell = await us.spell('😀');
      } catch (e) {
        st.fail(e, 'should not throw when the word is an emoji');
      }
    });

    t.test('Nodehun#spellSync(value)', function (st) {
      st.equal(
        us.spellSync('colour'),
        false,
        'should return `false` when a word is not correctly spelled'
      );

      st.equal(
        us.spellSync('color'),
        true,
        'should return `true` when a word is correctly spelled (1)'
      );

      st.equal(
        us.spellSync('c'),
        true,
        'should return `true` when a word is correctly spelled (2)'
      );

      st.equal(
        us.spellSync(' '),
        true,
        'should return `true` without word'
      );

      st.equal(
        us.spellSync('.'),
        true,
        'should return `true` for non-words'
      );

      st.equal(
        us.spellSync('ABDUL'),
        true,
        'should check for sentence-case when upper-case (ok)'
      );

      st.equal(
        us.spellSync('COLOUR'),
        false,
        'should check for sentence-case when upper-case (not ok)'
      );

      st.equal(
        us.spellSync('Color'),
        true,
        'should check for lower-case (ok)'
      );

      st.equal(
        us.spellSync('Colour'),
        false,
        'should check for lower-case (not ok)'
      );

      st.equal(
        us.spellSync('Colour'),
        false,
        'should check for lower-case (not ok)'
      );

      st.equal(
        nl.spellSync('DVD'),
        false,
        'should not check upper-case for sentence-case when KEEPCASE'
      );

      st.equal(
        nl.spellSync('dVd'),
        false,
        'should not check other casing for lower-case when KEEPCASE'
      );

      st.equal(
        nl.spellSync('eierlevendbarend'),
        true,
        'should support ONLYINCOMPOUND (ok)'
      );

      st.equal(
        nl.spellSync('eier'),
        false,
        'should support ONLYINCOMPOUND (not ok)'
      );

      st.equal(
        us.spellSync('21st'),
        true,
        'should support compounds (1)'
      );

      st.equal(
        us.spellSync('20st'),
        false,
        'should support compounds (2)'
      );

      st.equal(
        us.spellSync('20th'),
        true,
        'should support compounds (3)'
      );

      st.equal(
        us.spellSync('23st'),
        false,
        'should support compounds (4)'
      );

      st.equal(
        us.spellSync('23th'),
        false,
        'should support compounds (5)'
      );

      st.equal(
        us.spellSync('23rd'),
        true,
        'should support compounds (6)'
      );

      st.equal(
        us.spellSync('12th'),
        true,
        'should support compounds (7)'
      );

      st.equal(
        us.spellSync('22nd'),
        true,
        'should support compounds (8)'
      );

      st.end();
    });

    t.test('Nodehun#suggest(value)', async function (st) {
      try {
        const suggestions = await us.suggest('color');

        st.equal(
          suggestions,
          null,
          'should return `null` when correct'
        );
      } catch (e) {
        st.fail(e, 'should not throw');
      }

      try {
        const suggestions = await us.suggest('colour');
        st.deepEqual(
          suggestions.slice(0, 3),
          ['color', 'co lour', 'co-lour'],
          'should pass an array of `suggestions` when incorrect'
        );
      } catch (e) {
        st.fail(e, 'should not throw');
      }

      st.end();
    });

    t.test('Nodehun#suggestSync(value)', function (st) {
      st.deepEqual(
        us.suggestSync('color'),
        [],
        'should return an empty array when correct (1)'
      );

      st.deepEqual(
        us.suggestSync('c'),
        [],
        'should return an empty array when correct (2)'
      );

      st.deepEqual(
        us.suggestSync('colour').slice(0, 5),
        ['color', 'co lour', 'co-lour', 'col our', 'col-our'],
        'should suggest alternatives'
      );

      st.deepEqual(
        us.suggestSync('propper').slice(0, 5),
        ['proper', 'popper', 'prosper', 'cropper', 'propped'],
        'should suggest alternatives'
      );

      st.deepEqual(
        us.suggestSync(' '),
        [],
        'should return an empty array for empty values'
      );

      st.deepEqual(
        us.suggestSync('.'),
        [],
        'should return an empty array for non-words'
      );

      st.deepEqual(
        us.suggestSync('Colour').slice(0, 5),
        ['Co lour', 'Co-lour', 'Col our', 'Col-our', 'Color'],
        'should suggest alternatives for sentence-case'
      );

      st.deepEqual(
        us.suggestSync('COLOUR').slice(0, 5),
        ['COLOR', 'CO LOUR', 'CO-LOUR', 'COL OUR', 'COL-OUR'],
        'should suggest alternatives for upper-case'
      );

      st.deepEqual(
        us.suggestSync('coLOUR').slice(0, 5),
        ['col Our', 'co Lour', 'color', 'co-lour', 'col-our'],
        'should suggest alternatives for funky-case'
      );

      st.deepEqual(
        us.suggestSync('html'),
        ['HTML', 'ht ml', 'ht-ml'],
        'should suggest uppercase versions'
      );

      st.deepEqual(
        us.suggestSync('collor').slice(0, 5),
        ['color', 'collar', 'coll or', 'coll-or', 'collator'],
        'should suggest removals'
      );

      st.not(
        us.suggestSync('coor').indexOf('color'),
        -1,
        'should suggest additions'
      );

      st.deepEqual(
        us.suggestSync('cloor'),
        ['color', 'floor'],
        'should suggest switches'
      );

      st.deepEqual(
        us.suggestSync('cokor'),
        ['color', 'cork', 'Cork'],
        'should suggest insertions'
      );

      st.deepEqual(
        us.suggestSync('bulshit'),
        ['bullish'],
        'should not suggest alternatives marked with `NOSUGGEST`'
      );

      st.deepEqual(
        us.suggestSync('consize'),
        ['concise', 'con size', 'con-size'],
        'should suggest based on replacements'
      );

      st.deepEqual(
        us.suggestSync('npmnpmnpmnpmnpmnpmnpmnpmnpmnpmnpmnpmnpmnpmnpm'),
        [],
        'should not overflow on too long values'
      );

      st.end();
    });

    t.test('Nodehun#add(value)', async function (st) {
      st.plan(1);

      await us.add('npm')
      st.ok(us.spellSync('npm'), 'should now mark as correct');

      us.removeSync('npm');

      st.end();
    });

    t.test('Nodehun#addSync(value)', function (st) {
      st.equal(
        us.spellSync('npm'),
        false,
        'should initially be marked as incorrect'
      );

      st.deepEqual(
        us.suggestSync('npm').slice(0, 3),
        ['bpm', 'pm', 'rpm'],
        'should initially receive suggestions'
      );

      st.equal(
        us.addSync('npm'),
        true,
        'should return whether successful'
      );

      st.equal(
        us.spellSync('npm'),
        true,
        'should now mark as correct'
      );

      st.deepEqual(
        us.suggestSync('npm'),
        [],
        'should now no longer receive suggestions'
      );

      us.removeSync('npm');

      st.end();
    });

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

    t.test('Nodehun#remove(value, callbakc)', async function (st) {
      st.plan(4);

      us.addSync('npm');

      await us.remove('npm')
      
      st.equal(
        us.spellSync('npm'),
        false,
        'should now mark as incorrect'
      );
      
      st.deepEqual(
        us.suggestSync('npm').slice(0, 3),
        ['bpm', 'pm', 'rpm'],
        'should now receive suggestions'
      );

      st.end();
    });

    t.test('Nodehun#removeSync(value)', function (st) {
      us.addSync('npm');

      st.equal(
        us.removeSync('npm'),
        true,
        'should return whether successful'
      );

      st.equal(
        us.spellSync('npm'),
        false,
        'should now mark as incorrect'
      );

      st.deepEqual(
        us.suggestSync('npm').slice(0, 3),
        ['bpm', 'pm', 'rpm'],
        'should now receive suggestions'
      );

      st.end();
    });

    // TODO:
    // `stem`, `generate`, and `analyze`.

    t.end();
  });
}

/* Load dictionaries. */
(function () {
  var dictionaries = {};
  var count = 0;

  [EN_US, EN_GB, NL].forEach(function (name, index, context) {
    require('dictionary-' + name)(function (err, dictionary) {
      if (err) {
        throw err;
      }

      dictionaries[name] = dictionary;
      count++;

      if (count === context.length) {
        start(dictionaries);
      }
    });
  });
})();
