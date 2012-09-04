var nodehun = require('./../lib/index.js'),
	Dictionary = nodehun.Dictionary,
	mctime = require('microtime'),
	unit = typeof mctime !== "undefined" ? "μs":"ms",
	time = typeof mctime !== "undefined" ? mctime.now : Date.now;

var dict = new Dictionary('en_US');

dict.spellSuggest('colour',function(a,b){console.log('"colour" without addition',a,b)});
var timeWord = time();
console.log('adding "colour" to dictionary:', dict.addWord('colour',function(a,b){
	console.log('time adding word:',time() - timeWord, unit);
	console.log('added word:',a,b);
	dict.spellSuggest('colour',function(a,b){console.log('"colour" with addition',a,b)});
}) );

