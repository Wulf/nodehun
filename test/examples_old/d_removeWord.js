var debugFlag = process.argv.indexOf('debug') > -1;
var nodehun = require('./../build/' + (debugFlag ? 'Debug' : 'Release') + '/nodehun');
var fs = require('fs');
//var mctime = require('microtime');
unit = typeof mctime !== "undefined" ? "μs":"ms",
time = typeof mctime !== "undefined" ? mctime.now : Date.now;

var affbuf = fs.readFileSync(__dirname+'/dictionaries/en_US.aff');
var dictbuf = fs.readFileSync(__dirname+'/dictionaries/en_US.dic');

var dict = new nodehun(affbuf,dictbuf);
console.log('"color" before removal sync:', dict.spellSuggestSync('color'));
dict.removeWordSync('color');
console.log('"color" after removal sync:', dict.spellSuggestSync('color'));

var timeInit = time();
dict = new nodehun(affbuf,dictbuf);

dict.spellSuggest('color',function(err,a,b){
    if(!err)
	console.log('"color" without removal',a,b)
    var timeWord = time();
    dict.removeWord('color',function(err,a,b){
	console.log(err);
	console.log('time removing word:',time() - timeWord, unit);
	console.log('removed word:',a,b);
	dict.spellSuggest('color',function(err,a,b,c){console.log('"color" with removal',a,b,c);});
    });

});
