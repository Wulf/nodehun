{
  "targets": [
    {
      "target_name": "Nodehun",
      "sources": [ "src/index.cc" ],
      "include_dirs": ["<!@(node -p \"require('node-addon-api').include\")"],
      "dependencies": [
        "<!(node -p \"require('node-addon-api').gyp\")",
        "./hunspell/binding.gyp:hunspell"
      ],
      "defines": [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ]
    }
  ]
}
