var debugFlag = process.argv.indexOf('debug') > -1;
var nodehun = require('./../build/' + (debugFlag ? 'Debug' : 'Release') + '/nodehun');
var fs = require('fs');


var dict = new nodehun(fs.readFileSync(__dirname+'/dictionaries/en_US.aff'),fs.readFileSync(__dirname+'/dictionaries/en_US.dic'));

console.log('stemSync:telling', dict.stemSync('telling'));

dict.stem('telling',function(err,res){
    console.log(err, res);
});
