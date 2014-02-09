var nodehun = require('./../build/Release/nodehun');
var fs = require('fs');


var dict = new nodehun(fs.readFileSync(__dirname+'/dictionaries/en_US/en_US.aff'),fs.readFileSync(__dirname+'/dictionaries/en_US/en_US.dic'));
for(var i = 0; i < 40; i++)
    dict.spellSuggestions('calor',function(a,b,c){
	console.log(a,b,c);
    }.bind(undefined,i));
