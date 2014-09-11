var nodehun = require('./../build/Release/nodehun');
var fs = require('fs');


var dict = new nodehun(fs.readFileSync(__dirname+'/dictionaries/en_US/en_US.aff'),fs.readFileSync(__dirname+'/dictionaries/en_US/en_US.dic'));
for(var i = 0; i < 41; i++)
    dict.spellSuggestions('calor',function(err,a,b,c){
	if(!err)
	    console.log(a,b,c);
    }.bind(undefined,i));
