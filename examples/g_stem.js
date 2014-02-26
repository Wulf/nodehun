var nodehun = require('./../build/Release/nodehun');
var fs = require('fs');

var dict = new nodehun(fs.readFileSync(__dirname+'/dictionaries/en_US/en_US.aff'),fs.readFileSync(__dirname+'/dictionaries/en_US/en_US.dic'));

dict.analyze("word",function(res){
    dict.stem(res,function(res2){
	console.log(res,' to ',res2);
    });
});
