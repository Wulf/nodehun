var nodehun = require('./../build/Release/nodehun');
var fs = require('fs');
//var mctime = require('microtime');
unit = typeof mctime !== "undefined" ? "μs":"ms",
time = typeof mctime !== "undefined" ? mctime.now : Date.now;

var affbuf = fs.readFileSync(__dirname+'/dictionaries/en_US/en_US.aff');
var dictbuf = fs.readFileSync(__dirname+'/dictionaries/en_US/en_US.dic');

var timeInit = time();
var dict = new nodehun(affbuf,dictbuf);
console.log('time to initialize dictionary class:',time() - timeInit,unit);
dict.spellSuggest('Tacoma',function(err,a,b){
    if(!err)
	console.log('misspelling "Tacoma"',a,b,'time to receive result:',time() - timeInit,unit)
});
dict.spellSuggestions('calor',function(err,a,b){
    if(!err)
	console.log('misspelling "calor"',a,b)
});

dict.spellSuggest('color',function(err,a,b){
    if(!err)
	console.log('correct "color"',a,b)
});
dict.spellSuggestions('color',function(err,a,b){
    if(!err)
	console.log('correct "color"',a,b)
});
