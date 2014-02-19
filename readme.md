Nodehun
=======

Installation
------------
Nodehun has no "node_module" dependencies (yet), so it can either be installed via npm or simply checked out of git. You'll need [node-gyp](https://github.com/TooTallNate/node-gyp) to build. Nodehun should work on Windows or Unix. You'll also need to make sure that libuv source
code is on your system. Usually having node installed is enough, but there are weird cases.
	
	npm install nodehun
	

Introduction
------------
Yes there are already two nodejs spell checkers based of off hunspell, but one doesn't seem to even be supported anymore, and the other seems to only support simple spelling suggestions. Nodehun aims to expose as much of hunspell's functionality as possible in an easy to understand and maintain way, while also offering additional functionality not even present in hunspell.

Spell Suggest and Initialization
--------------------------------
Initializing nodehun is very easy, simply add the buffer of an affix and dictionary file as the first two arguments of the constructor. The mechanics of the dictionaries nodehun processes is fairly simple to understand. Nodehun ships with US english and Canadian English (look in the examples folder), but tons of languages are available for free at [open office](http://extensions.services.openoffice.org/dictionary), see the readme file in the dictionaries folder for more directions. Of course you don't need to use the filesystem you could use a distributed data store to store the dictionaries. Please do not actually use `readFileSync`.
	     
	var nodehun = require('nodehun');
	var affbuf = fs.readFileSync(somedirectory+'/en_US.aff');
	var dictbuf = fs.readFileSync(somedirectory+'/en_US.dic');
	var dict = new nodehun(affbuf,dictbuf);
	dict.spellSuggest('color',function(a,b){
		console.log(a,b);
		// because "color" is a defined word in the US English dictionary
		// the output will be: true, null
	});
	
	dict.spellSuggest('calor',function(a,b){
		console.log(a,b);
		// because "calor" is not a defined word in the US English dictionary
		// the output will be: false, "carol"
	});

Spell Suggestions
-----------------
Nodehun also offers a method that returns an array of words that could possibly match a misspelled word, ordered by most likely to be correct.
	
	var nodehun = require('nodehun');
	var affbuf = fs.readFileSync(somedirectory+'/en_US.aff');
	var dictbuf = fs.readFileSync(somedirectory+'/en_US.dic');
	var dict = new nodehun(affbuf,dictbuf);
	
	dict.spellSuggestions('color',function(a,b){
		console.log(a,b);
		// because "color" is a defined word in the US English dictionary
		// the output will be: true, []
	});

	dict.spellSuggest('calor',function(a,b){
		console.log(a,b);
		// because "calor" is not a defined word in the US English dictionary
		// the output will be: false, [ 'carol','valor','color','cal or','cal-or','caloric','calorie']
	});
	
Add Dictionary
--------------
Nodehun also can add another dictionary on top of an existing dictionary object at runtime (this means it is not permanent) in order to merge two dictionaries. Once again, please do not actually use `readFileSync`.
	
	var nodehun = require('nodehun');
	var affbuf = fs.readFileSync(somedirectory+'/en_US.aff');
	var dictbuf = fs.readFileSync(somedirectory+'/en_US.dic');
	var dictbuf2 = fs.readFileSync(somedirectory+'/en_CA.dic');
	var dict = new nodehun(affbuf,dictbuf);

	dict.spellSuggest('colour',function(a,b){
		console.log(a,b);
		// because "colour" is not a defined word in the US English dictionary
		// the output will be: false, "color"
	});
	
	dict.addDictionary(dictbuf2,function(added){
		console.log(added);
		// because the Canadian English dictionary exists,
		// the output will be: true
		USDictionary.spellSuggest('colour',function(a,b){
			console.log(a,b);
			// because "colour" is a defined word in the Canadian English dictionary
			// the output will be: true, null
		});
	});
	
Add Word
--------
Nodehun can also add a single word to a dictionary at runtime (this means it is not permanent) in order to have a custom runtime dictionary. If you know anything about Hunspell you can also add flags to the word.

	var nodehun = require('nodehun');
	var affbuf = fs.readFileSync(somedirectory+'/en_US.aff');
	var dictbuf = fs.readFileSync(somedirectory+'/en_US.dic');
	var dict = new nodehun(affbuf,dictbuf);
	
	dict.spellSuggest('colour',function(a,b){
		console.log(a,b);
		// because "colour" is not a defined word in the US English dictionary
		// the output will be: false, "color"
	});
	
	dict.addWord('colour',function(a,b){
		console.log(a,b);
		// if the method succeeded then
		// the output will be: true, 'colour'
		dict.spellSuggest('colour',function(a,b){
			console.log(a,b);
			// because "colour" has been added to the US dictionary object.
			// the output will be: true, null
		});
		
	});
	
Remove Word
-----------
Nodehun can also remove a single word from a dictionary at runtime (this means it is not permanent) in order to have a custom runtime dictionary. If you know anything about Hunspell this method will ignore flags and just strip words that match.
	
	var nodehun = require('nodehun');
	var affbuf = fs.readFileSync(somedirectory+'/en_US.aff');
	var dictbuf = fs.readFileSync(somedirectory+'/en_US.dic');
	var dict = new nodehun(affbuf,dictbuf);
	
	dict.spellSuggest('color',function(a,b){
		console.log(a,b);
		// because "color" is a defined word in the US English dictionary
		// the output will be: true, null
	});
	
	dict.removeWord('color',function(a,b){
		console.log(a,b);
		// if the method succeeded then
		// the output will be: true, 'color'
		dict.spellSuggest('color',function(a,b){
			console.log(a,b);
			// because "color" has been removed from the US dictionary object.
			// the output will be: false, "colors"
			// note that plurals are considered separte words.
		});
		
	});
	
A Note About Creating Dictionaries
----------------------------------

If you want to create a new Hunspell dictionary you will need a base affix file. I recommend simply using one of the base affix files from the open office dictionaries for the language you are creating a dictionary for. Once you get around to creating a dictionary read the hunspell documentation to learn how to properly flag the words. However, my guess is that the vast majority of people creating dictionaries out there will be creating a dictionary of proper nouns. Proper nouns simply require the "M" flag. This is what a dictionary of proper nouns might look like:
   
   Aachen/M
   aardvark/SM
   Aaren/M
   Aarhus/M
   Aarika/M
   Aaron/M

Notice that the "S" flag denotes a proper noun that isn't capitalized, otherwise look in the docs.