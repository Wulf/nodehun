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
      "defines": [ "NAPI_DISABLE_CPP_EXCEPTIONS" ],
      # set standard to C++ 17
      "cflags_cc": [
        "-std=c++1z"
      ],
      # and on MAC
      "xcode_settings": {
        "OTHER_CFLAGS": [
          "-std=c++1z"
        ]
      },
      "msvs_settings": {
        "VCCLCompilerTool": {
          "AdditionalOptions": [ "-std:c++1z", ],
        },
      },
    }
  ]
}
