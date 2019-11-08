Hunspell Distributed
--------------------
This fork of hunspell is specifically refactored to make passing around file-paths irrelvant, raw buffers are passed instead. Additionally, all extraneous tools have been removed. This allows hunspell to run on a distributed system much more easily.

Some History
------------
This fork of hunspell is significant in that a few core things about hunspell have been changed (some interface classes have been created, etc) to allow buffers of dictionaries rather than references to them to be passed around. The main project hasn't wanted these changes so this fork will keep them here. The project originally started so that hunspell could run distributed on node.js, and be able to read dictionaries that were stored on a database rather than on the local file system. This has worked well, but the time to split has come, as other projects have expressed interest in using this code.