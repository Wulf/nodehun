var nodehun = require('./../lib/index.js'),
	Dictionary = nodehun.Dictionary,
	mctime = require('microtime'),
	unit = typeof mctime !== "undefined" ? "μs":"ms",
	time = typeof mctime !== "undefined" ? mctime.now : Date.now;
	
var dict = new Dictionary('en_US');

dict.spellSuggest('colour',function(a,b){console.log('"colour" without en_CA',a,b)});
var timeAdd = time();
console.log('adding CA:', dict.addDictionary('en_CA',function(a,b){
	console.log('time to add dictionary:',time() - timeAdd, unit);
	console.log('dictionary added:',a,b);
	dict.spellSuggest('colour',function(a,b){console.log('"colour" with en_CA',a,b)});
}) );