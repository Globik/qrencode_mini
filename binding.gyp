{"targets":[{"target_name":"qrencode_mini","sources":["qrencode_mini.c"],
"dependencies":[
"./deps/libqrencode/qrencode.gyp:liblibqrenca"
#"../addon1/deps/libqrencode/qrencode.qyp:liblibqrenca"
],
"cflags":["-std=c99","-O3"],
"include_dirs":[
#"../addon1/deps/libqrencode"
"./deps/libqrencode/libqrencode"
],
"libraries":["-lpng",
#"-L/home/globik/libqrencode-js/simple_async/build/Release -lqrenca"
#"-L/home/globik/libqrencode-js/simple_async/build/Release/libqrenca.a"
"-L./build/Release/libqrenca.a"
],
"defines":["HAVE_PNG"]
} 
]
}