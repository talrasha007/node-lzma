{
  "targets": [
    {
      "target_name": "liblzma",
      "variables": {
        "lzma_ver": "1505"
      },
      "type": "static_library",
      "standalone_static_library": 1,
      "defines": [
      ],
      "sources": [
        "lzma<(lzma_ver)/Alloc.c",
        "lzma<(lzma_ver)/LzFind.c",
        "lzma<(lzma_ver)/LzFindMt.c",
        "lzma<(lzma_ver)/LzmaDec.c",
        "lzma<(lzma_ver)/LzmaEnc.c",
        "lzma<(lzma_ver)/LzmaLib.c",
        "lzma<(lzma_ver)/Threads.c"
      ],
      "conditions": [
        [
          "OS == 'win'", {
            "msvs_settings": {
              "VCCLCompilerTool": {
                "EnableFunctionLevelLinking": "true",
                "DisableSpecificWarnings": [ "4024", "4047", "4146", "4244", "4267", "4996" ]
              }
            }
          }
        ],
        [
          "OS == 'mac'", {
            "xcode_settings": {
              "WARNING_CFLAGS": [
                "-Wno-format-extra-args"
              ]
            }
          }
        ]
      ]
    }
  ]
}