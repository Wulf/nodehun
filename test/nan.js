const Nodehun = require('..');

require('dictionary-en-us')(function (err, dictionary) {
  if (err) throw err;

  const instance = new Nodehun(dictionary.aff, dictionary.dic);
  console.log('Nodehun NAN Implementation Tests');
  console.log('--------------------------------');
  const emoji = '😀'
  console.log('instance.correctSync("drinkabl")\t',instance.correctSync("drinkabl"));
  console.log('instance.suggestSync("drinkabl")\t', instance.suggestSync('drinkabl'));
  console.log('instance.suggestSync("'+emoji+'")\t\t', instance.suggestSync(emoji));
  console.log('instance.correctSync("'+emoji+'")\t\t', instance.correctSync(emoji));
  instance.correct('drinkabl', function(error, isCorrect) {
    console.log('instance.correct("drinkabl")\t\t', [error, isCorrect]);
  })
  instance.suggest('drinkable', function(error, correct, suggestions) {
    console.log('instance.suggest("drinkable")\t\t', [error, correct, suggestions]);
  })
  instance.suggest('drinkabl', function(error, correct, suggestions) {
    console.log('instance.suggest("drinkabl")\t\t', [error, correct, suggestions]);
  })
});
