var nodehun = require('./../build/Release/nodehun');
var fs = require('fs');
//var mctime = require('microtime');
unit = typeof mctime !== "undefined" ? "μs":"ms",
time = typeof mctime !== "undefined" ? mctime.now : Date.now;

var affbuf = fs.readFileSync(__dirname+'/dictionaries/en_US/en_US.aff');
var dictbuf = fs.readFileSync(__dirname+'/dictionaries/en_US/en_US.dic');

var timeInit = time();
var dict = new nodehun(affbuf,dictbuf);

dict.spellSuggest('color',function(err,a,b){
    if(!err)
	console.log('"color" without removal',a,b)
});
var timeWord = time();
dict.removeWord('color',function(err,a,b){
    console.log(err);
    console.log('time removing word:',time() - timeWord, unit);
    console.log('removed word:',a,b);
    dict.spellSuggest('color',function(a,b){console.log('"color" with removal',a,b)});
});
