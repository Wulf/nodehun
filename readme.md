Nodehun
=======
Introduction
------------
Nodehun aims to expose as much of hunspell's functionality as possible in an easy to understand and maintainable way, while also maintaining the performance characteristics expected of a responsible node module. 

Warning on Versions
-------------------
The method signatures of version 0.XX.XX differs from the current versions. The first argument in these versions was a success parameter, when really an error should be null or passed if something went wrong. Here is an example of how they would differ:

```js
//0.XX.XX version:
dict.addWord('foo',function(success, word){
   console.log(success, word);
   // if the method succeeded then
   // the output will be : true, 'foo'
n}); 
//1.XX.XX/2.XX.XX version:
dict.addWord('xxxxxxx', function(err, word){
   console.log(err);
   // if the method succeeded then
   // the output will be: null, 'foo'
});
```

Installation
------------

	npm install nodehun


Spell Suggest and Initialization
--------------------------------
Initializing nodehun is very easy, simply add the buffer of an affix and dictionary file as the first two arguments of the constructor. The mechanics of the dictionaries nodehun processes is fairly simple to understand. Nodehun ships with US english and Canadian English (look in the examples folder), but tons of languages are available for free at [open office](http://extensions.services.openoffice.org/dictionary), see the readme file in the dictionaries folder for more directions. Of course you don't need to use the filesystem you could use a distributed data store to store the dictionaries. Please do not actually use `readFileSync`.

```js
var nodehun = require('nodehun');
var affbuf = fs.readFileSync(somedirectory+'/en_US.aff');
var dictbuf = fs.readFileSync(somedirectory+'/en_US.dic');
var dict = new nodehun(affbuf,dictbuf);
dict.spellSuggest('color',function(err, correct, suggestion, origWord){
	console.log(err, correct, suggestion, origWord);
	// because "color" is a defined word in the US English dictionary
	// the output will be: null, true, null, 'color'
});

dict.spellSuggest('calor',function(err, correct, suggestion, origWord){
	console.log(err, correct, suggestion, origWord);
	// because "calor" is not a defined word in the US English dictionary
	// the output will be: null, false, "carol", 'calor'
});
```

Checking for Correctness
------------------------
Nodehun offers a method that returns true or false if the passed word exists in the dictionary, i.e. is "correct".

```js
var nodehun = require('nodehun');
var affbuf = fs.readFileSync(somedirectory+'/en_US.aff');
var dictbuf = fs.readFileSync(somedirectory+'/en_US.dic');
var dict = new nodehun(affbuf,dictbuf);

dict.isCorrect('color',function(err, correct, origWord){
	console.log(err, correct, origWord);
	// because "color" is a defined word in the US English dictionary
	// the output will be: null, true, "color"
});
```

Spell Suggestions
-----------------
Nodehun also offers a method that returns an array of words that could possibly match a misspelled word, ordered by most likely to be correct.

```js
var nodehun = require('nodehun');
var affbuf = fs.readFileSync(somedirectory+'/en_US.aff');
var dictbuf = fs.readFileSync(somedirectory+'/en_US.dic');
var dict = new nodehun(affbuf,dictbuf);

dict.spellSuggestions('color',function(err, correct, suggestions, origWord){
	console.log(err, correct, suggestions, origWord);
	// because "color" is a defined word in the US English dictionary
	// the output will be: null, true, [], 'color'
});

dict.spellSuggestions('calor',function(err, correct, suggestions, origWord){
	console.log(err, correct, suggestions, origWord);
	// because "calor" is not a defined word in the US English dictionary
	// the output will be: null, false, [ 'carol','valor','color','cal or','cal-or','caloric','calorie'], 'calor'
});
```

Add Dictionary
--------------
Nodehun also can add another dictionary on top of an existing dictionary object at runtime (this means it is not permanent) in order to merge two dictionaries. Once again, please do not actually use `readFileSync`.

```js
var nodehun = require('nodehun');
var affbuf = fs.readFileSync(somedirectory+'/en_US.aff');
var dictbuf = fs.readFileSync(somedirectory+'/en_US.dic');
var dictbuf2 = fs.readFileSync(somedirectory+'/en_CA.dic');
var dict = new nodehun(affbuf,dictbuf);

dict.spellSuggest('colour',function(err, correct, suggestion, origWord){
	console.log(err, correct, suggestion, origWord);
	// because "colour" is not a defined word in the US English dictionary
	// the output will be: null, false, "color", 'colour'
});

dict.addDictionary(dictbuf2,function(err){
	if(!err)
		USDictionary.spellSuggest('colour',function(err, correct, suggestion, origWord){
			console.log(err, correct, suggestion, origWord);
			// because "colour" is a defined word in the Canadian English dictionary
			// the output will be: null, true, null, 'colour'
		});				
});
```

Add Word
--------
Nodehun can also add a single word to a dictionary at runtime (this means it is not permanent) in order to have a custom runtime dictionary. If you know anything about Hunspell you can also add flags to the word.

```js
var nodehun = require('nodehun');
var affbuf = fs.readFileSync(somedirectory+'/en_US.aff');
var dictbuf = fs.readFileSync(somedirectory+'/en_US.dic');
var dict = new nodehun(affbuf,dictbuf);

dict.spellSuggest('colour',function(err, correct, suggestion, origWord){
	console.log(err, correct, suggestions, origWord);
	// because "colour" is not a defined word in the US English dictionary
	// the output will be: null, false, "color", 'colour'
});

dict.addWord('colour',function(err, word){
	if(!err)
		dict.spellSuggest('colour',function(err, correct, suggestions, origWord){
			console.log(err, correct, suggestions, origWord);	
			// because "colour" has been added to the US dictionary object.
			// the output will be: true, null, 'colour'
		});

});
```

Remove Word
-----------
Nodehun can also remove a single word from a dictionary at runtime (this means it is not permanent) in order to have a custom runtime dictionary. If you know anything about Hunspell this method will ignore flags and just strip words that match.

```js
var nodehun = require('nodehun');
var affbuf = fs.readFileSync(somedirectory+'/en_US.aff');
var dictbuf = fs.readFileSync(somedirectory+'/en_US.dic');
var dict = new nodehun(affbuf,dictbuf);

dict.spellSuggest('color',function(err, correct, suggestion, origWord){
	console.log(err, correct, suggestion, origWord);
	// because "color" is a defined word in the US English dictionary
	// the output will be: null, true, null, 'color'
	dict.removeWord('color',function(err, word){
		if(!err)			
			dict.spellSuggest('color',function(err, correct, suggestion, origWord){
				console.log(err, correct, suggestion, origWord);
				// because "color" has been removed from the US dictionary object.
				// the output will be: null, false, "colors", 'color'
				// note that plurals are considered separte words.
			});

	});
});
```

Stem
----
Nodehun exposes the Hunspell `stem` function which analyzes the roots of words. Consult the Hunspell documentation for further understanding.

```js
var nodehun = require('nodehun');
var affbuf = fs.readFileSync(somedirectory+'/en_US.aff');
var dictbuf = fs.readFileSync(somedirectory+'/en_US.dic');
var dict = new nodehun(affbuf,dictbuf);

dict.stem('telling',function(err, stems){
	console.log(err, stems);
	// the output will be: null, [telling, tell]
});
```

Analyze
----
Nodehun exposes the Hunspell `analyze` function which analyzes a word and return a morphological analysis. Consult the Hunspell documentation for further understanding.

```js
var nodehun = require('nodehun');
var affbuf = fs.readFileSync(somedirectory+'/en_US.aff');
var dictbuf = fs.readFileSync(somedirectory+'/en_US.dic');
var dict = new nodehun(affbuf,dictbuf);

dict.analyze('telling',function(err, fields){
	console.log(err, fields);
	// with the provided dictionaries, the output will be: null [ ' st:telling ts:0', ' st:tell ts:0 al:told is:Vg' ]
});
```

Generate
----
Nodehun exposes the Hunspell `generate` function which generates a varition of a word by matching the morphological structure of another word. Consult the Hunspell documentation for further understanding.

```js
var nodehun = require('nodehun');
var affbuf = fs.readFileSync(somedirectory+'/en_US.aff');
var dictbuf = fs.readFileSync(somedirectory+'/en_US.dic');
var dict = new nodehun(affbuf,dictbuf);

dict.generate('telling', 'ran', function(err,res){
    console.log(err, res);
	// the output will be: null [ 'told' ]

	dict.generate('told', 'run', function(err,res){
		console.log(err, res);
	// the output will be: null [ 'tell' ]
	});
});
```

Asynchronous Invocation
-----------------------
Initializing the nodehun object can be bumpy if you're doing it a lot. A large dictionary can take up to 100ms to initialize. This is obviously unacceptable in applications that need on-the-fly dictionary creation. Therefore there is a static method on the nodehun object that allows you to initialize the dictionary object asynchronously, like so:

```js
var nodehun = require('nodehun');
var affbuf = fs.readFileSync(somedirectory+'/en_US.aff');
var dictbuf = fs.readFileSync(somedirectory+'/en_US.dic');

nodehun.createNewNodehun(affbuf,dictbuf,function(err,dict){
	if(!err)
		dict.spellSuggest('color',function(err, correct, suggestion){
			console.log(err, correct, suggestion);
			// because "color" is a defined word in the US English dictionary
			// the output will be: null, true, null
		});
});
```

A Warning on Synchronous Methods
-----------------------------
There are synchronous versions of all the methods listed above, but they are not doucmented as they are only present for people who really know and understand what they are doing. I highly recommend looking at the C++ source code if you are going to use these methods in a production environment as the locks involved with them can create some counterintuitive situations. For example, if you were to remove a word synchronously while many different suggestion threads were working in the backgound the remove word method could take seconds to complete while it waits to take control of the read-write lock. This is obviously disastrous in a situation where you would be servicing many requests.

A Note About Open Office Dictionaries
-------------------------------------
All files must be UTF-8 to work! When you download [open office dictionaries](http://cgit.freedesktop.org/libreoffice/dictionaries/tree/) don't assume that the file is UTF-8 just because it is being served as a UTF-8 file. You may have to convert the file using the `iconv` unix utility (easy enough to do) to UTF-8 in order for the files to work.

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