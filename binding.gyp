{
	"targets": [
		{
			"target_name": "native_libnice",
			"sources": [
				"native/module.cpp",
				"native/Agent.cpp",
			],
			"include_dirs"  : [
				"<!(node -e \"require('nan')\")"
			],
			"defines": [
				#'DO_DEBUG'
			],
			'cflags': [
				'-std=c++0x',
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
                    "-mmacosx-version-min=10.7",
                    "<!@(pkg-config --cflags nice glib-2.0)",
                ]
            }
		}
	],
}
