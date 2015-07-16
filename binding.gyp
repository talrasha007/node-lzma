{
  "targets": [
    {
      "target_name": "lzma",
	  "include_dirs" : [
        "<!(node -e \"require('nan')\")",
        "deps/lzma1505"
      ],
      "dependencies": [
        "<(module_root_dir)/deps/lzma.gyp:liblzma"
      ],
      "sources": [
        "src/module.cc"
      ],
      "conditions": [
        [
          "OS == 'win'", {
            "defines": [
              "_HAS_EXCEPTIONS=0"
            ],
            'msvs_settings': {
              'VCCLCompilerTool': {
                'RuntimeTypeInfo': 'false',
                'EnableFunctionLevelLinking': 'true',
                'ExceptionHandling': '2',
                'DisableSpecificWarnings': [ '4267' ]
              }
            }
          }
        ],
        [
          "OS=='linux'", {
            "cflags_cc": [ "-std=c++11" ]
          }
        ],
        [
          'OS == "mac"', {
            'xcode_settings': {
              'MACOSX_DEPLOYMENT_TARGET': '10.7',
              'OTHER_CPLUSPLUSFLAGS': [
                  '-std=c++11' ,
                  '-stdlib=libc++'
              ]
            }
          }
        ]
      ]
    }
  ]
}