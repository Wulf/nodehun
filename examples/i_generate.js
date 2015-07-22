var debugFlag = process.argv.indexOf('debug') > -1;
var nodehun = require('./../build/' + (debugFlag ? 'Debug' : 'Release') + '/nodehun');
var fs = require('fs');


var dict = new nodehun(fs.readFileSync(__dirname+'/dictionaries/en_US.aff'),fs.readFileSync(__dirname+'/dictionaries/en_US.dic'));
console.log('generateSync:telling,ran:',dict.generateSync('telling', 'ran'));
console.log('generateSync:told,run:',dict.generateSync('told', 'run'));

dict.generate('telling', 'ran', function(err,res){
	console.log('passed tense of telling is', res[0], err);

	dict.generate('told', 'run', function(err,res){
		console.log('present tense of told is', res[0], err);
	});
});
