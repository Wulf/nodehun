{
    'target_defaults': {
	'default_configuration': 'Debug',
	'configurations': {
	    'Debug': {
		'defines': [ 'DEBUG', '_DEBUG' ],
		'msvs_settings': {
		    'VCCLCompilerTool': {
			'RuntimeLibrary': 1, # static debug
		    },
		},
	    },
	    'Release': {
		'defines': [ 'NDEBUG' ],
		'msvs_settings': {
		    'VCCLCompilerTool': {
			'RuntimeLibrary': 0, # static release
		    },
		},
	    }
	},
	'msvs_settings': {
	    'VCCLCompilerTool': {
	    },
	    'VCLibrarianTool': {
	    },
	    'VCLinkerTool': {
		'GenerateDebugInformation': 'true',
	    },
	},
    },

    'targets': [
	{
	    'target_name': 'hunspell',
	    'type': 'static_library',
	    'include_dirs': [ 'hunspell/src/hunspell' ],
	    'defines': [ 'HUNSPELL_STATIC' ],
	    'direct_dependent_settings': {
		'include_dirs': [ 'hunspell/src/hunspell' ],
		'defines': [ 'HUNSPELL_STATIC' ],
	    },
	    'cflags': [ '-O3' ],
	    'sources': [
		'hunspell/src/hunspell/affentry.cxx',
		'hunspell/src/hunspell/affentry.hxx',
		'hunspell/src/hunspell/affixmgr.cxx',
		'hunspell/src/hunspell/affixmgr.hxx',
		'hunspell/src/hunspell/atypes.hxx',
		'hunspell/src/hunspell/baseaffix.hxx',
		'hunspell/src/hunspell/csutil.cxx',
		'hunspell/src/hunspell/csutil.hxx',
		'hunspell/src/hunspell/filemgr.cxx',
		'hunspell/src/hunspell/filemgr.hxx',
		'hunspell/src/hunspell/hashmgr.cxx',
		'hunspell/src/hunspell/hashmgr.hxx',
		'hunspell/src/hunspell/htypes.hxx',
		'hunspell/src/hunspell/hunspell.cxx',
		'hunspell/src/hunspell/hunspell.hxx',
		'hunspell/src/hunspell/hunzip.cxx',
		'hunspell/src/hunspell/hunzip.hxx',
		'hunspell/src/hunspell/langnum.hxx',
		'hunspell/src/hunspell/phonet.cxx',
		'hunspell/src/hunspell/phonet.hxx',
		'hunspell/src/hunspell/replist.cxx',
		'hunspell/src/hunspell/replist.hxx',
		'hunspell/src/hunspell/suggestmgr.cxx',
		'hunspell/src/hunspell/suggestmgr.hxx',
		'hunspell/src/hunspell/w_char.hxx'
	    ],
	    'conditions': [
		['OS=="win"', {
	            'include_dirs': [ 'configs' ],
	            'sources': [
			'configs/config-win.h',
	            ],
	        }, {
	            'sources': [
			'configs/config-other.h',
	            ],
	        }],
	    ]
	}
    ]
}