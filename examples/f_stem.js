var nodehun = require('./../build/Release/nodehun');
var fs = require('fs');


var dict = new nodehun(fs.readFileSync(__dirname+'/dictionaries/en_US/en_US.aff'),fs.readFileSync(__dirname+'/dictionaries/en_US/en_US.dic'));
dict.stem('telling',function(err,res){
    console.log(err, res);
});
