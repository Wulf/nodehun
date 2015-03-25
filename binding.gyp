{
  'targets': [
    {
      'target_name': 'nodehun',
      'sources': [
        'src/nodehun.cpp'
      ],
      'cflags': [ '-O3' ],
      'dependencies': [
        'src/hunspell/binding.gyp:hunspell'
      ],
    },
  ]
}
