'use strict';

/* Dependencies. */
var test = require('tape');
var Hun = require('..');

/* Constants. */
var EN_GB = 'en-gb';
var EN_US = 'en-us';
var NL = 'nl';

/**
 * Start the tests with loaded `dictionaries`.
 *
 * @param {Object} dictionaries - Loaded dictionaries.
 */
function start(dictionaries) {
  test('nodehun()', function (t) {
    var us;
    var gb;
    var nl;

    t.equal(
      typeof Hun,
      'function',
      'should expose a function'
    );

    t.throws(
      function () {
        Hun();
      },
      /^Error: Use the new operator to create an instance of this object\.$/,
      'should throw without `new`'
    );

    t.throws(
      function () {
        new Hun();
      },
      /^Error: Constructor requires two arguments\.$/,
      'should throw when missing parameters'
    );

    us = new Hun(dictionaries[EN_US].aff, dictionaries[EN_US].dic);
    gb = new Hun(dictionaries[EN_GB].aff, dictionaries[EN_GB].dic);
    nl = new Hun(dictionaries[NL].aff, dictionaries[NL].dic);

    t.equal(
      us instanceof Hun,
      true,
      'should create a new instance from a dictionary (1)'
    );

    t.equal(
      gb instanceof Hun,
      true,
      'should create a new instance from a dictionary (2)'
    );

    // t.test('Hun#wordCharacters()', function (st) {
    //   st.equal(
    //     us.wordCharacters(),
    //     '0123456789',
    //     'should return the defined word-characters'
    //   );
    //
    //   st.equal(
    //     gb.wordCharacters(),
    //     null,
    //     'should return `null` when not defined'
    //   );
    //
    //   st.end();
    // });

    t.test('Hun#isCorrect(value, callback)', function (st) {
      st.plan(5);

      us.isCorrect('colour', function (err, correct) {
        st.ifErr(err, 'should not throw');

        st.equal(
          correct,
          false,
          'should pass `false` when a word is not correctly spelled'
        );
      });

      gb.isCorrect('colour', function (err, correct) {
        st.ifErr(err, 'should not throw');

        st.equal(
          correct,
          true,
          'should pass `true` when a word is correctly spelled'
        );
      });

      us.isCorrect('😀', function(err, correct) {
        st.ifErr(err, 'should not throw when the word is an emoji');
      });
    });

    t.test('Hun#isCorrectSync(value)', function (st) {
      st.equal(
        us.isCorrectSync('colour'),
        false,
        'should return `false` when a word is not correctly spelled'
      );

      st.equal(
        us.isCorrectSync('color'),
        true,
        'should return `true` when a word is correctly spelled (1)'
      );

      st.equal(
        us.isCorrectSync('c'),
        true,
        'should return `true` when a word is correctly spelled (2)'
      );

      st.equal(
        us.isCorrectSync(' '),
        true,
        'should return `true` without word'
      );

      st.equal(
        us.isCorrectSync('.'),
        true,
        'should return `true` for non-words'
      );

      st.equal(
        us.isCorrectSync('ABDUL'),
        true,
        'should check for sentence-case when upper-case (ok)'
      );

      st.equal(
        us.isCorrectSync('COLOUR'),
        false,
        'should check for sentence-case when upper-case (not ok)'
      );

      st.equal(
        us.isCorrectSync('Color'),
        true,
        'should check for lower-case (ok)'
      );

      st.equal(
        us.isCorrectSync('Colour'),
        false,
        'should check for lower-case (not ok)'
      );

      st.equal(
        us.isCorrectSync('Colour'),
        false,
        'should check for lower-case (not ok)'
      );

      st.equal(
        nl.isCorrectSync('DVD'),
        false,
        'should not check upper-case for sentence-case when KEEPCASE'
      );

      st.equal(
        nl.isCorrectSync('dVd'),
        false,
        'should not check other casing for lower-case when KEEPCASE'
      );

      st.equal(
        nl.isCorrectSync('eierlevendbarend'),
        true,
        'should support ONLYINCOMPOUND (ok)'
      );

      st.equal(
        nl.isCorrectSync('eier'),
        false,
        'should support ONLYINCOMPOUND (not ok)'
      );

      st.equal(
        us.isCorrectSync('21st'),
        true,
        'should support compounds (1)'
      );

      st.equal(
        us.isCorrectSync('20st'),
        false,
        'should support compounds (2)'
      );

      st.equal(
        us.isCorrectSync('20th'),
        true,
        'should support compounds (3)'
      );

      st.equal(
        us.isCorrectSync('23st'),
        false,
        'should support compounds (4)'
      );

      st.equal(
        us.isCorrectSync('23th'),
        false,
        'should support compounds (5)'
      );

      st.equal(
        us.isCorrectSync('23rd'),
        true,
        'should support compounds (6)'
      );

      st.equal(
        us.isCorrectSync('12th'),
        true,
        'should support compounds (7)'
      );

      st.equal(
        us.isCorrectSync('22nd'),
        true,
        'should support compounds (8)'
      );

      st.end();
    });

    t.test('Hun#spellSuggestions(value, callback)', function (st) {
      st.plan(8);

      us.spellSuggestions('color', function (err, correct, suggestions, origWord) {
        st.ifErr(err, 'should not throw');

        st.equal(
          correct,
          true,
          'should pass `correct: true` when correct'
        );

        st.deepEqual(
          suggestions,
          [],
          'should pass an empty `suggestions` array when correct'
        );

        st.equal(
          origWord,
          'color',
          'should pass `origWord` when correct'
        );
      });

      us.spellSuggestions('colour', function (err, correct, suggestions, origWord) {
        st.ifErr(err, 'should not throw');

        st.equal(
          correct,
          false,
          'should pass `correct: false` when incorrect'
        );

        st.deepEqual(
          suggestions.slice(0, 3),
          ['color', 'co lour', 'co-lour'],
          'should pass an array of `suggestions` when incorrect'
        );

        st.equal(
          origWord,
          'colour',
          'should pass `origWord` when incorrect'
        );
      });
    });

    t.test('Hun#spellSuggestionsSync(value)', function (st) {
      st.deepEqual(
        us.spellSuggestionsSync('color'),
        [],
        'should return an empty array when correct (1)'
      );

      st.deepEqual(
        us.spellSuggestionsSync('c'),
        [],
        'should return an empty array when correct (2)'
      );

      st.deepEqual(
        us.spellSuggestionsSync('colour').slice(0, 5),
        ['color', 'co lour', 'co-lour', 'col our', 'col-our'],
        'should suggest alternatives'
      );

      st.deepEqual(
        us.spellSuggestionsSync('propper').slice(0, 5),
        ['proper', 'popper', 'prosper', 'cropper', 'propped'],
        'should suggest alternatives'
      );

      st.deepEqual(
        us.spellSuggestionsSync(' '),
        [],
        'should return an empty array for empty values'
      );

      st.deepEqual(
        us.spellSuggestionsSync('.'),
        [],
        'should return an empty array for non-words'
      );

      st.deepEqual(
        us.spellSuggestionsSync('Colour').slice(0, 5),
        ['Co lour', 'Co-lour', 'Col our', 'Col-our', 'Color'],
        'should suggest alternatives for sentence-case'
      );

      st.deepEqual(
        us.spellSuggestionsSync('COLOUR').slice(0, 5),
        ['COLOR', 'CO LOUR', 'CO-LOUR', 'COL OUR', 'COL-OUR'],
        'should suggest alternatives for upper-case'
      );

      st.deepEqual(
        us.spellSuggestionsSync('coLOUR').slice(0, 5),
        ['col Our', 'co Lour', 'color', 'co-lour', 'col-our'],
        'should suggest alternatives for funky-case'
      );

      st.deepEqual(
        us.spellSuggestionsSync('html'),
        ['HTML', 'ht ml', 'ht-ml'],
        'should suggest uppercase versions'
      );

      st.deepEqual(
        us.spellSuggestionsSync('collor').slice(0, 5),
        ['color', 'collar', 'coll or', 'coll-or', 'collator'],
        'should suggest removals'
      );

      st.not(
        us.spellSuggestionsSync('coor').indexOf('color'),
        -1,
        'should suggest additions'
      );

      st.deepEqual(
        us.spellSuggestionsSync('cloor'),
        ['color', 'floor'],
        'should suggest switches'
      );

      st.deepEqual(
        us.spellSuggestionsSync('cokor'),
        ['color', 'cork', 'Cork'],
        'should suggest insertions'
      );

      st.deepEqual(
        us.spellSuggestionsSync('bulshit'),
        ['bullish'],
        'should not suggest alternatives marked with `NOSUGGEST`'
      );

      st.deepEqual(
        us.spellSuggestionsSync('consize'),
        ['concise', 'con size', 'con-size'],
        'should suggest based on replacements'
      );

      st.deepEqual(
        us.spellSuggestionsSync('npmnpmnpmnpmnpmnpmnpmnpmnpmnpmnpmnpmnpmnpmnpm'),
        [],
        'should not overflow on too long values'
      );

      st.end();
    });

    t.test('Hun#addWord(value, callback)', function (st) {
      st.plan(3);

      us.addWord('npm', function (err, ok) {
        st.ifErr(err);
        st.equal(ok, 'npm', 'should pass the added word');
        st.ok(us.isCorrectSync('npm'), 'should now mark as correct');

        us.removeWordSync('npm');
      });
    });

    t.test('Hun#addWordSync(value)', function (st) {
      st.equal(
        us.isCorrectSync('npm'),
        false,
        'should initially be marked as incorrect'
      );

      st.deepEqual(
        us.spellSuggestionsSync('npm').slice(0, 3),
        ['bpm', 'pm', 'rpm'],
        'should initially receive suggestions'
      );

      st.equal(
        us.addWordSync('npm'),
        true,
        'should return whether successful'
      );

      st.equal(
        us.isCorrectSync('npm'),
        true,
        'should now mark as correct'
      );

      st.deepEqual(
        us.spellSuggestionsSync('npm'),
        [],
        'should now no longer receive suggestions'
      );

      us.removeWordSync('npm');

      st.end();
    });

    // t.test('Hun#addWordSync(value, model)', function (st) {
    //   /* `azc` is a Dutch word only properly spelled
    //    * in its lower-case form. */
    //   st.equal(
    //     nl.addWordSync('npm', 'azc'),
    //     nl,
    //     'should return the context object'
    //   );
    //
    //   st.equal(nl.isCorrectSync('npm'), true, 'should match affixes (1)');
    //   st.equal(nl.isCorrectSync('NPM'), false, 'should match affixes (2)');
    //   st.equal(nl.isCorrectSync('Npm'), false, 'should match affixes (3)');
    //
    //   nl.removeWordSync('npm');
    //
    //   st.end();
    // });

    t.test('Hun#removeWord(value, callbakc)', function (st) {
      st.plan(4);

      us.addWordSync('npm');

      us.removeWord('npm', function (err, origWord) {
        st.ifErr(err);
        st.equal(origWord, 'npm', 'should pass the operated on word');
        st.equal(
          us.isCorrectSync('npm'),
          false,
          'should now mark as incorrect'
        );
        st.deepEqual(
          us.spellSuggestionsSync('npm').slice(0, 3),
          ['bpm', 'pm', 'rpm'],
          'should now receive suggestions'
        );
      });
    });

    t.test('Hun#removeWordSync(value)', function (st) {
      us.addWordSync('npm');

      st.equal(
        us.removeWordSync('npm'),
        true,
        'should return whether successful'
      );

      st.equal(
        us.isCorrectSync('npm'),
        false,
        'should now mark as incorrect'
      );

      st.deepEqual(
        us.spellSuggestionsSync('npm').slice(0, 3),
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
