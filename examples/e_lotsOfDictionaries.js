var nodehun = require('./../build/Release/nodehun');
var fs = require('fs');
var dicts = [];

for(var i = 0, l = 100; i <= l; i++){
    console.log(i);
    dicts.push(new nodehun(fs.readFileSync(__dirname+'/dictionaries/en_US/en_US.aff'),fs.readFileSync(__dirname+'/dictionaries/en_US/en_US.dic')));
}
for(i = 0; i <= l; i++){
    dicts[i].spellSuggestions('calor',function(a,b){console.log('misspelling "calor"',a,b)});
}

