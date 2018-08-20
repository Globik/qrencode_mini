//const ad=require('./build/Release/qrencode_mini');
const ad=require('./index.js');
const {makeMeasurable}=require('performance-meter');
const ob3={background_color:'76eec6',foreground_color:'ff0000', level:"q",dot_size:6,margin:2,micro:0,version:4}
//const ob3={foreground_color:'ff0000',level:'q',dot_size:6,margin:2,micro:0,version:4}
// -s dot_size
// level{high=QR_ECLEVEL_H,q=QR_ECLEVEL_Q,m=QR_ECLEVEL_M,l=QR_ECLEVEL_L} => DEFAULT=L(low)
const ob2={};
//let bn=ad.setOptions(ob);
//console.log('bn: ',bn);
// todo: eightbit can be hardcoded, and version?(must be too)
//micro is optional dot_size, margin are too. Colors also optional. Level? One default and then very specific
//#76eec6
const str="mama";
const str2="bitcoin:1BMXmqU3fZ8PVjPbxgeenEX93YYf74bjeB?amount50&label=John&message=Donation%20for%20project";
console.log("str2 length: ",str2.length);
const buf=Buffer.from(str2);
console.log("input buf length: ",buf.length);
//         argv[0] argv[1]  argv[2]
for(var i=0;i<1;i++){
//ad.qrencode(buf,ob3,function(err,val){console.log('error: ',err);console.log('valA: ',val.toString());});

ad.qrencode(buf,ob3).then(function(val){console.log('valA: ',val.toString());}).catch(function(err){console.log('error: ',err);})
}
