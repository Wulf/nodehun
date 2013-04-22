var nodehun = require('./../lib/index.js'),
Dictionary = nodehun.Dictionary;
var fs = require('fs'),
mctime = require('microtime'),
unit = typeof mctime !== "undefined" ? "μs":"ms",
time = typeof mctime !== "undefined" ? mctime.now : Date.now;

var affbuf = fs.readFileSync(__dirname+'/../dictionaries/en_US/en_US.aff').toString();
var dictbuf = fs.readFileSync(__dirname+'/../dictionaries/en_US/en_US.dic').toString();
var timeInit = time();
//var dict = new Dictionary('en_US');
var dict = new Dictionary(affbuf,dictbuf);
console.log('time to initialize dictionary class:',time() - timeInit,unit);
timeInit = time();
dict.spellSuggest('Tacoma',function(a,b){console.log('misspelling "Tacoma"',a,b,'time to receive result:',time() - timeInit,unit)});
dict.spellSuggestions('calor',function(a,b){console.log('misspelling "calor"',a,b)});

dict.spellSuggest('color',function(a,b){console.log('correct "color"',a,b)});
dict.spellSuggestions('color',function(a,b){console.log('correct "color"',a,b)});
