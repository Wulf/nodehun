var debugFlag = process.argv.indexOf('debug') > -1;
var nodehun = require('./../build/' + (debugFlag ? 'Debug' : 'Release') + '/nodehun');
var fs = require('fs');
//var mctime = require('microtime');
unit = typeof mctime !== "undefined" ? "μs":"ms",
time = typeof mctime !== "undefined" ? mctime.now : Date.now;

var affbuf = fs.readFileSync(__dirname+'/dictionaries/en_US.aff');
var dictbuf = fs.readFileSync(__dirname+'/dictionaries/en_US.dic');

var timeInit = time();
var dict = new nodehun(affbuf,dictbuf);
console.log('time to initialize dictionary class:',time() - timeInit,unit);
dict.spellSuggest('Tacoma',function(err,a,b,c){
    if(!err)
	console.log('misspelling "Tacoma"',a,b,c,'time to receive result:',time() - timeInit,unit)
});

try{
    console.log('sync Tacoma:',dict.spellSuggestSync('Tacoma'));
}catch(e){
    console.log(e);
}

dict.spellSuggestions('calor',function(err,a,b,c){
    if(!err)
	console.log('misspelling "calor"',a,b,c)
});

try{
    console.log('sync calor:', dict.spellSuggestionsSync('calor'));
}catch(e){
    console.log(e);
}

dict.spellSuggest('color',function(err,a,b,c){
    if(!err)
	console.log('correct "color"',a,b,c)
});
dict.spellSuggestions('color',function(err,a,b,c){
    if(!err)
	console.log('correct "color"',a,b,c)
});
