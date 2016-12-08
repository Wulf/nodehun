var debugFlag = process.argv.indexOf('debug') > -1;
var nodehun = require('./../build/' + (debugFlag ? 'Debug' : 'Release') + '/nodehun');
//var mctime = require('microtime');
unit = typeof mctime !== "undefined" ? "μs":"ms",
time = typeof mctime !== "undefined" ? mctime.now : Date.now;

var dict = new nodehun(__dirname+'/dictionaries/en_US.aff', __dirname+'/dictionaries/en_US.dic');
console.log('"colour" without en_CA sync', dict.spellSuggestSync('colour'));

dict.addDictionarySync(__dirname+'/dictionaries/en_CA.dic')

console.log('"colour" with en_CA sync', dict.spellSuggestSync('colour'));

var timeInit = time();
dict = new nodehun(__dirname+'/dictionaries/en_US.aff',__dirname+'/dictionaries/en_US.dic');
console.log('time to initialize dictionary class:',time() - timeInit,unit);


dict.spellSuggest('colour',function(err, a,b,c){
    if(!err) 
	console.log('"colour" without en_CA',a,b,c)
});
var timeAdd = time();
dict.addDictionary(__dirname+'/dictionaries/en_CA.dic',function(err){
    console.log("error:",err);
    console.log('time to add dictionary:',time() - timeAdd, unit);
    dict.spellSuggest('colour',function(err,a,b,c){if(!err)console.log('"colour" with en_CA',a,b,c)});
});


