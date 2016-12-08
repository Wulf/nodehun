var debugFlag = process.argv.indexOf('debug') > -1;
var nodehun = require('./../build/' + (debugFlag ? 'Debug' : 'Release') + '/nodehun');

var dict = new nodehun(__dirname+'/dictionaries/en_US.aff',__dirname+'/dictionaries/en_US.dic');
for(var i = 0; i < 100; i++)
    process.nextTick(function(i){
	dict.spellSuggestions('calor',function(err, a, b, c){
	    console.log(i, err, a, b, c);
	});
    }.bind(undefined, i));
