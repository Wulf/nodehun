var nodehun = require('./../build/Release/nodehun');
var fs = require('fs');

var affbuf = fs.readFileSync(__dirname+'/dictionaries/en_US/en_US.aff');
var dictbuf = fs.readFileSync(__dirname+'/dictionaries/en_US/en_US.dic');

nodehun.createNewNodehun(affbuf, dictbuf,function(err, dict){

    if(!err)
	dict.spellSuggest('color', function(err, correct, suggestions){
	    if(!err)
		console.log('correct "color"', correct, suggestions)
	});
});
