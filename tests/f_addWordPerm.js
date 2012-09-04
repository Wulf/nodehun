var nodehun = require('./../lib/index.js'),
	Dictionary = nodehun.Dictionary,
	mctime = require('microtime'),
	unit = typeof mctime !== "undefined" ? "μs":"ms",
	time = typeof mctime !== "undefined" ? mctime.now : Date.now;

var dict = new Dictionary('ds_Places');
dict.spellSuggest('Tukwilla',function(a,b){console.log('"Tukwilla" without new word',a,b)});
nodehun.addWordsPerm('ds_Places',["Tukwilla"],function(err,success){
	var dict2 = new Dictionary('ds_Places');
	dict.spellSuggest('Tukwilla',function(a,b){console.log('"Tukwilla" with new word',a,b)});
})
		

