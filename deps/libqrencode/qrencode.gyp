{
"targets":[
{"target_name":"liblibqrenca","type":"static_library",
"sources":[
"./libqrencode/bitstream.c",
"./libqrencode/mask.c",
"./libqrencode/qrencode.c",
"./libqrencode/qrspec.c",
"./libqrencode/mmask.c",
"./libqrencode/rsecc.c",
"./libqrencode/mqrspec.c",
"./libqrencode/qrinput.c",
"./libqrencode/split.c"
],
"direct_dependent_settings":{"include_dirs":["./libqrencode"]},
 "defines":["HAVE_PNG","STATIC_IN_RELEASE=static","MAJOR_VERSION=4","MICRO_VERSION=0","MINOR_VERSION=0","VERSION=\"4.0.0\""]
}]}