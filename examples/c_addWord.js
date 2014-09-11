var nodehun = require('./../build/Release/nodehun');
var fs = require('fs');
//var mctime = require('microtime');
unit = typeof mctime !== "undefined" ? "μs":"ms",
time = typeof mctime !== "undefined" ? mctime.now : Date.now;

var affbuf = fs.readFileSync(__dirname+'/dictionaries/en_US/en_US.aff');
var dictbuf = fs.readFileSync(__dirname+'/dictionaries/en_US/en_US.dic');

var timeInit = time();
var dict = new nodehun(affbuf,dictbuf);

dict.spellSuggest('colour',function(err,a,b){
    if(!err)
	console.log('"colour" without addition',a,b)
});
var timeWord = time();
dict.addWord('colour',function(err,a,b){
    if(!err)
	console.log(err);
    console.log('time adding word:',time() - timeWord, unit);
    console.log('added word:',a,b);
    dict.spellSuggest('colour',function(err,a,b){
	if(!err)
	    console.log('"colour" with addition',a,b)
    });
});

