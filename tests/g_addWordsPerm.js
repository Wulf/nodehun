var nodehun = require('./../lib/index.js'),
	Dictionary = nodehun.Dictionary,
	mctime = require('microtime'),
	unit = typeof mctime !== "undefined" ? "μs":"ms",
	time = typeof mctime !== "undefined" ? mctime.now : Date.now;
		

var speed = time();
var jitter = speed;
nodehun.addWordsPerm('ds_Places',['somethinga','nothing',["something",'wc'],'art','aaw'],function(err,words){
	console.log('time:',time() - speed,unit);
	console.log('error:',err);
	console.log('words:',words);
	var dict = new Dictionary('ds_Places');
	dict.spellSuggestions('nothing',function(a,b){console.log('misspelling "nothing"',a,b)});
});
console.log('jitter:',time() - jitter,unit);