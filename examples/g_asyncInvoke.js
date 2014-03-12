var nodehun = require('./../build/Release/nodehun');
var fs = require('fs');

var affbuf = fs.readFileSync(__dirname+'/dictionaries/en_US/en_US.aff');
var dictbuf = fs.readFileSync(__dirname+'/dictionaries/en_US/en_US.dic');

nodehun.createNewNodehun(affbuf,dictbuf,function(dict){
    dict.spellSuggest('color',function(a,b){console.log('correct "color"',a,b)});
});
