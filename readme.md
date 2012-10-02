Nodehun
=======

Installation
------------
Nodehun has no "node_module" dependencies (yet), so it can either be installed via npm or simply checked out of git. You'll need [node-gyp](https://github.com/TooTallNate/node-gyp) to build. Nodehun should work on Windows or Unix. You'll also need to make sure that libuv source
code is on your system. Usually having node installed is enough, but there are weird cases.
	
	npm install nodehun
	cd src
	node-gyp configure
	node-gyp build
	

Introduction
------------
Yes there are already two nodejs spell checkers based of off hunspell, but one doesn't seem to even be supported anymore, and the other seems to only support simple spelling suggestions. Nodehun aims to expose as much of hunspell's functionality as possible in an easy to understand and maintain way, while also offering additional functionality not even present in hunspell.

Spell Suggest and Initialization
-------------------------------
Initializing nodehun is very easy, it will automatically find the dictionary you are looking for as long as it is inside the dictionaries folder (nodehun ships with US english and Canadian English, but tons of languages are available for free at [open office](http://extensions.services.openoffice.org/dictionary), you should be able to just drop any of open office's dictionary folders into nodehun's dictionary folder and it should automatically work, see the readme file in the dictionaries folder for more directions). From initialization there are only a few built in objects that come with nodehun, most of the functionality you will use are methods in the built in object "Dictionary". Simple spell suggest is very easy.
	
	var nodehun = require('nodehun'),
		USDictionary = new nodehun.Dictionary('en_US');
		
	USDictionary.spellSuggest('color',function(a,b){
		console.log(a,b);
		// because "color" is a defined word in the US English dictionary
		// the output will be: true, null
	});
	
	USDictionary.spellSuggest('calor',function(a,b){
		console.log(a,b);
		// because "calor" is not a defined word in the US English dictionary
		// the output will be: false, "carol"
	});
	
Spell Suggestions
-----------------
Nodehun also offers a method that returns an array of words that could possibly match a misspelled word, ordered by most likely to be correct.
	
	var nodehun = require('nodehun'),
		USDictionary = new nodehun.Dictionary('en_US');
	
	USDictionary.spellSuggestions('color',function(a,b){
		console.log(a,b);
		// because "color" is a defined word in the US English dictionary
		// the output will be: true, []
	});

	USDictionary.spellSuggest('calor',function(a,b){
		console.log(a,b);
		// because "calor" is not a defined word in the US English dictionary
		// the output will be: false, [ 'carol','valor','color','cal or','cal-or','caloric','calorie']
	});
	
Add Dictionary
--------------
Nodehun also can add another dictionary on top of an existing dictionary object at runtime (this means it is not permanent) in order to merge two dictionaries.
	
	var nodehun = require('nodehun'),
		USDictionary = new nodehun.Dictionary('en_US');
	
	USDictionary.spellSuggest('colour',function(a,b){
		console.log(a,b);
		// because "colour" is not a defined word in the US English dictionary
		// the output will be: false, "color"
	});
	
	USDictionary.addDictionary('en_CA',function(a,b){
		console.log(a,b);
		// because the Canadian English dictionary exists,
		// the output will be: true, 'en_CA'
		USDictionary.spellSuggest('colour',function(a,b){
			console.log(a,b);
			// because "colour" is a defined word in the Canadian English dictionary
			// the output will be: true, null
		});
		
	});
	
Add Word
--------
Nodehun can also add a single word to a dictionary at runtime (this means it is not permanent) in order to have a custom runtime dictionary. If you know anything about Hunspell you can also add flags to the word.
	
	var nodehun = require('nodehun'),
		USDictionary = new nodehun.Dictionary('en_US');
	
	USDictionary.spellSuggest('colour',function(a,b){
		console.log(a,b);
		// because "colour" is not a defined word in the US English dictionary
		// the output will be: false, "color"
	});
	
	USDictionary.addWord('colour',function(a,b){
		console.log(a,b);
		// if the method succeeded then
		// the output will be: true, 'colour'
		USDictionary.spellSuggest('colour',function(a,b){
			console.log(a,b);
			// because "colour" has been added to the US dictionary object.
			// the output will be: true, null
		});
		
	});
	
Remove Word
-----------
Nodehun can also remove a single word from a dictionary at runtime (this means it is not permanent) in order to have a custom runtime dictionary. If you know anything about Hunspell this method will ignore flags and just strip words that match.
	
	var nodehun = require('nodehun'),
		USDictionary = new nodehun.Dictionary('en_US');
	
	USDictionary.spellSuggest('color',function(a,b){
		console.log(a,b);
		// because "color" is a defined word in the US English dictionary
		// the output will be: true, null
	});
	
	USDictionary.removeWord('color',function(a,b){
		console.log(a,b);
		// if the method succeeded then
		// the output will be: true, 'color'
		USDictionary.spellSuggest('color',function(a,b){
			console.log(a,b);
			// because "color" has been removed from the US dictionary object.
			// the output will be: false, "colors"
			// note that plurals are considered separte words.
		});
		
	});
	
Add Dictionary Permanently
--------------------------
I have personally added some additional functionality to Hunspell and that is the ability to create custom dictionaries permanently. This will most likely be useful with domain specific services like search engines where a list of obscure proper nouns needs to be deemed correct. This method is NOT specific to a Dictionary object, it is a method on the nodehun object itself, it will most likely be useful in conjunction with the "addWordsPerm" method (see below). There are a couple of things to consider with this method as it takes some parameters. 

Hunspell compatible dictionaries have both an affix (.aff) and dictionary (.dic) file. These files are used to help the algorithm stem the words it receives from the (.dic) file appropriately. Obviously in creating a custom dictionary file you don't want to have to deal with thinking about how stemming will work, so the "addDictionaryPerm" forces you to specify a base dictionary from which you will derive your affix file (.aff). It also has another parameter which allows you to build off the specified dictionaries "dictionary" (.dic) file so that you will effectively be creating a copy of the dictionary that you specified in the parameter (this may be useful if you want to compile a custom dictionary that has both your words and some other dictionary's as well ex American English). If you don't specify to build of the base dictionary's "dictionary" (.dic) file you will create a blank .dic file that has no words in it, but you may add words to it by using the "addWordsPerm" method.
	
	var require('nodehun');
	
	nodehun.addDictionaryPerm('myNewDictionary1',function(err){
		if(!err){
			console.log("This dictionary uses the `en_US` dictionary for it's affix (.aff) file, but it's dictionary (.dic) file is blank.");
		}
		else{
			console.log("There was an error adding the dictionary:",err);
		}
	},'en_US',false);
	
	nodehun.addDictionaryPerm('myNewDictionary1',function(err){
		if(!err){
			console.log("This dictionary uses the `en_US` dictionary for it's affix (.aff) file and it's dictionary (.dic) file.");
		}
		else{
			console.log("There was an error adding the dictionary:",err);
		}
	},'en_US',true);
	
Add Word Permanently
--------------------
The "addWordPerm" method also deserves some careful consideration. Hunspell's dictionary (.dic) file contains a list of alphabetically sorted words that have special flags associated with them. Words in this file do not necessarily need these flags but they are helpful. If you want to get to know these flags I suggest that you look through the Hunspell documentation. I will talk about one briefly though.

The "M" flag is a flag that Hunspell recognizes as a flag fro a proper noun, if it sees that flag it will do special things like become case-sensitive (not in its search but in its return result), and ignore stemming rules (for the most part). I am assuming that the bulk of why anybody would want to add a word permanently to any of these dictionaries is to add proper nouns that's why I'm highlighting it here. Given that here is how the method works.

The "addWordsPerm" method first accepts a string that represents a dictionary as its argument, this can be any dictionary in the dictionaries folder. The second argument will be an array of words (or one word) that can be either strings or arrays. If an element is a string it will be added to the dictionary without any flags, if is an array the first element of that array will be assumed to be a word and the second will be assumed to be its flag(s). The third argument is a callback function.
	
	var nodehun = require('nodehun');
	
	nodehun.addWordsPerm('en_US',["boss",["Appleton","M"],["Madison","M"]],function(err,words){
		if(!err){
			console.log(words);
			// The words argument of the callback will contain all the words of the second argument
			// that were added to the specified  dictionary permanently. If a word wasn't added
			// and now error is passed that means the word was already in the dictionary. Because
			// "boss" is probably already in the en_US dictionary
			// the output will be: ["Appleton","Madison"]
			// note that the flags aren't returned in the words argument.
		}
		else{
			console.log("There was an error adding the words");
		}
	});
	
	