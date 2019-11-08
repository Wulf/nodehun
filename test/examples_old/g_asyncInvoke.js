var debugFlag = process.argv.indexOf('debug') > -1;
var nodehun = require('./../build/' + (debugFlag ? 'Debug' : 'Release') + '/nodehun');
var fs = require('fs');

var affbuf = fs.readFileSync(__dirname+'/dictionaries/en_US.aff');
var dictbuf = fs.readFileSync(__dirname+'/dictionaries/en_US.dic');

nodehun.createNewNodehun(affbuf, dictbuf,function(err, dict){

    if(!err)
	dict.spellSuggest('color', function(err, correct, suggestions, origWord){
	    if(!err)
		console.log('correct "color"', correct, suggestions, origWord)
	});
});
