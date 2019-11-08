var debugFlag = process.argv.indexOf('debug') > -1;
var nodehun = require('./../build/' + (debugFlag ? 'Debug' : 'Release') + '/nodehun');
var fs = require('fs');

var dict = new nodehun(fs.readFileSync(__dirname+'/dictionaries/en_US.aff'),fs.readFileSync(__dirname+'/dictionaries/en_US.dic'));
var words = ['original', 'roach', 'erasee', 'come', 'consol', 'argumnt', 'gage',
	     'libary', 'lisence', 'principal', 'realy', 'license', 'suprise', 'writting'];

var handleMisspellings = function(err, correct, origWord, suggestions) {
    if (err) throw err;
    if (correct)
	console.log(origWord + ' is spelled correctly!');
    else
	console.log(origWord + ' not recognized. Suggestions: ' + suggestions);
}

words.forEach(function(w) {
    dict.spellSuggestions(w, handleMisspellings);
});
