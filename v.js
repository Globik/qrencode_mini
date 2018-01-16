//simple_async.cc from simple_async
const ad=require('./build/Release/qrencode_mini');
const {makeMeasurable}=require('performance-meter');
const ob={/*background_color:'76eec6',*/foreground_color:'ff0000', level:"q",dot_size:6,margin:2,micro:0,version:4}
// -s dot_size
// level{high=QR_ECLEVEL_H,q=QR_ECLEVEL_Q,m=QR_ECLEVEL_M,l=QR_ECLEVEL_L} => DEFAULT=L(low)
const ob2={};
//let bn=ad.setOptions(ob);
//console.log('bn: ',bn);
// todo: eightbit can be hardcoded, and version?(must be too)
//micro is optional dot_size, margin are too. Colors also optional. Level? One default and then very specific
//#76eec6
const str="mama";
const buf=Buffer.from(str);
//         argv[0] argv[1]  argv[2]
ad.qrencode(buf,{type:"nu"},function(err,val){console.log('error: ',err);console.log('valA: ',val.toString());});
var k=0;
//dura();
function dura(){
var t=setInterval(function(){
	
ad.qrencode(buf,{type:"nu"},function(err,val){
	k++;
	console.log('error: ',err);console.log('valA: ',val.toString('base64').substring(0,30));
//console.log("K: ",k);
//if(k==300){su();}
});
},0);
}
function su(){clearInterval(t);}
var boo=makeMeasurable(boo);
//boo(1);

//margin,micro,8,caseinsetiv,color1,color2
function boo(n){
for(var i=0;i<n;i++){
ad.qrenc(buf,{type:"nu"},function(err,val){console.log('valA: ',val.toString('base64').substring(0,16));});	
}
}
var e=0;
/*
setTimeout(function(){
ad.Test(buf,ob,function(err,val){console.log('valA: ',val.toString('base64').substring(0,16));});	
},4000);
*/
// SYNC:  1.540ms=n1 4.318ms=n5  35.373ms=n50            //using synchronous napi and napi create_buffer_copy folder=simple_sync
// ASYNC: 0.788=n1   1.030ms=n5  2.209ms=n=50            //napi_create_async_work and napi create_buffer_copy folder=simple_async
// ASYNC: 0.704      1.070       9.208        2.443=n100 // using C++ uv_queue_work and new Buffer  folder=addon1
// another module qr-node.js boo=n1: 21.223ms boo=n5: 24.156ms much slower 20x?? through process.child.on_data 
// qrcode-js.js:
//boo=n1: 345.011ms boo=n5: 1117.979ms

//ASYNC:               0.98
//boo: 1.054ms=n1 boo: 0.116ms=n1 if from cache

/*
valgrind --leak-check=yes node b.js

 definitely lost: 0 bytes in 0 blocks
==10308==    indirectly lost: 0 bytes in 0 blocks
==10308==      possibly lost: 16,478 bytes in 277 blocks
==10308==    still reachable: 370,915 bytes in 1,559 blocks
==10308==         suppressed: 0 bytes in 0 blocks
==10308== Reachable blocks (those to which a pointer was found) are not shown.
==10308== To see them, rerun with: --leak-check=full --show-leak-kinds=all
==10308== 
==10308== For counts of detected and suppressed errors, rerun with: -v
==10308== ERROR SUMMARY: 222 errors from 222 contexts (suppressed: 0 from 0)
*/