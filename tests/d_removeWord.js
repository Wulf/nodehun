var nodehun = require('./../lib/index.js'),
	Dictionary = nodehun.Dictionary,
	mctime = require('microtime'),
	unit = typeof mctime !== "undefined" ? "μs":"ms",
	time = typeof mctime !== "undefined" ? mctime.now : Date.now;

var dict = new Dictionary('en_US');

dict.spellSuggest('color',function(a,b){console.log('"color" without removal',a,b)});
var timeWord = time();
console.log('removing "color" from dictionary:', dict.removeWord('color',function(a,b){
	console.log('time removing word:',time() - timeWord, unit);
	console.log('removed word:',a,b);
	dict.spellSuggest('color',function(a,b){console.log('"color" with removal',a,b)});
}) );
