var nodehun = require('./../lib/index.js'),
	Dictionary = nodehun.Dictionary,
	mctime = require('microtime'),
	unit = typeof mctime !== "undefined" ? "μs":"ms",
	time = typeof mctime !== "undefined" ? mctime.now : Date.now,
fs = require('fs');

var affbuf = fs.readFileSync(__dirname+'/../dictionaries/en_US/en_US.aff').toString();
var dictbuf = fs.readFileSync(__dirname+'/../dictionaries/en_US/en_US.dic').toString();
var dictbuf2 = fs.readFileSync(__dirname+'/../dictionaries/en_CA/en_CA.dic').toString();

var timeInit = time();
var dict = new Dictionary(affbuf,dictbuf);
console.log('time to initialize dictionary class:',time() - timeInit,unit);


dict.spellSuggest('colour',function(a,b){console.log('"colour" without en_CA',a,b)});
var timeAdd = time();
console.log('adding CA:', dict.addDictionary(dictbuf2,true,function(a,b){
	console.log('time to add dictionary:',time() - timeAdd, unit);
	console.log('dictionary added:',a,'buffer dictionary');
	dict.spellSuggest('colour',function(a,b){console.log('"colour" with en_CA',a,b)});
}) );

timeInit = time();
dict = new Dictionary('en_US');
console.log('time to initialize dictionary class:',time() - timeInit,unit);


dict.spellSuggest('colour',function(a,b){console.log('"colour" without en_CA',a,b)});
timeAdd = time();
console.log('adding CA:', dict.addDictionary('en_CA',function(a,b){
	console.log('time to add dictionary:',time() - timeAdd, unit);
	console.log('dictionary added:',a,b);
	dict.spellSuggest('colour',function(a,b){console.log('"colour" with en_CA',a,b)});
}) );
