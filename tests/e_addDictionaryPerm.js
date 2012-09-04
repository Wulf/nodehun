var nodehun = require('./../lib/index.js'),
	Dictionary = nodehun.Dictionary,
	mctime = require('microtime'),
	unit = typeof mctime !== "undefined" ? "μs":"ms",
	time = typeof mctime !== "undefined" ? mctime.now : Date.now;

nodehun.addDictionaryPerm('ds_Places',function(err){
	if(!err){
		var dict = new Dictionary('ds_Places');
		dict.spellSuggest('color',function(a,b){console.log('"color" without new dictionary',a,b)});
	}
	console.log(err);
},'en_US');

