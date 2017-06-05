{
  'targets': [{
    'target_name': 'Nodehun',
    'sources': [
      'src/NodehunObject.cpp'
    ],
    'cflags': [ '-O3' ],
    'dependencies': [
      'src/binding.gyp:hunspell'
    ],
    'include_dirs': [
      "<!(node -e \"require('nan')\")"
    ]
  }]
}
