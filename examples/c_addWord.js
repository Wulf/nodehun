var debugFlag = process.argv.indexOf('debug') > -1;
var nodehun = require('./../build/' + (debugFlag ? 'Debug' : 'Release') + '/nodehun');
var fs = require('fs');
//var mctime = require('microtime');
unit = typeof mctime !== "undefined" ? "μs":"ms",
time = typeof mctime !== "undefined" ? mctime.now : Date.now;
var affbuf = fs.readFileSync(__dirname+'/dictionaries/en_US.aff');
var dictbuf = fs.readFileSync(__dirname+'/dictionaries/en_US.dic');

var dict = new nodehun(affbuf,dictbuf);
console.log('"colour" before addition sync:', dict.spellSuggestSync('colour'));
dict.addWordSync('colour');
console.log('"colour" after addition sync:', dict.spellSuggestSync('colour'));

var timeInit = time();
dict = new nodehun(affbuf,dictbuf);

dict.spellSuggest('colour',function(err,a,b,c){
    if(!err)
	console.log('"colour" without addition',a,b,c)
    var timeWord = time();
    dict.addWord('colour',function(err,a,b){
	if(!err)
	    console.log(err);
	console.log('time adding word:',time() - timeWord, unit);
	console.log('added word:',a,b);
	dict.spellSuggest('colour',function(err,a,b,c){
	    if(!err)
		console.log('"colour" with addition',a,b,c)
	});
    });


});
