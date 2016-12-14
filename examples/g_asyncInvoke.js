var debugFlag = process.argv.indexOf('debug') > -1;
var nodehun = require('./../build/' + (debugFlag ? 'Debug' : 'Release') + '/nodehun');

nodehun.createNewNodehun(__dirname+'/dictionaries/en_US.aff', __dirname+'/dictionaries/en_US.dic',function(err, dict){

    if(!err)
	dict.spellSuggest('color', function(err, correct, suggestions, origWord){
	    if(!err)
		console.log('correct "color"', correct, suggestions, origWord)
	});
});
