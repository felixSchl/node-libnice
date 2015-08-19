{
    "targets": [
        {
            "target_name": "native_libnice",
            "sources": [
                "native/helper.cpp",
                "native/module.cpp",
                "native/stream.cpp",
                "native/agent.cpp",
            ],
            "include_dirs"  : [
                "<!(node -e \"require('nan')\")"
            ],
            "defines": [
                #'DO_DEBUG'
            ],
            'cflags': [
                '-std=c++11',
                '<!@(pkg-config --cflags nice glib-2.0)',
                '-Wall',
                '-g',
            ],
            'ldflags': [
                '<!@(pkg-config --libs nice glib-2.0)',
            ],
            'xcode_settings': {
                'OTHER_CFLAGS': [
                    "-std=c++11",
                    "-stdlib=libc++",
                    "-mmacosx-version-min=10.7",
                    "<!@(pkg-config --cflags nice glib-2.0)",
                ],
                'OTHER_LDFLAGS': [
                    "<!@(pkg-config --libs nice glib-2.0)",
                ]
            }
        }
    ],
}
