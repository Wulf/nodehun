var debugFlag = process.argv.indexOf('debug') > -1;
var nodehun = require('./../build/' + (debugFlag ? 'Debug' : 'Release') + '/nodehun');
var fs = require('fs');


var dict = new nodehun(fs.readFileSync(__dirname+'/dictionaries/en_US.aff'),fs.readFileSync(__dirname+'/dictionaries/en_US.dic'));
console.log('isCorrectSync:telling:', dict.isCorrectSync('telling'));
console.log('isCorrectSync:telled:', dict.isCorrectSync('telled'));

dict.isCorrect('telling', function(err, correct, word){
    console.log(err, correct, word);
});

dict.isCorrect('telled', function(err, correct, word){
    console.log(err, correct, word);
});
