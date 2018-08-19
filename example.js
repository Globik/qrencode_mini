const {setOptions,qrencode}=require('qrencode_mini');//require('./index.js');
var b=setOptions({margin:2,micro:1,version:3,foreground_color:"#fa0000"});
//node example
console.log("B: ",b);
let str="mama";
let buf=Buffer.from(str);
qrencode(buf).then(function(data){console.log('data: ',data.toString('base64'));}).catch(function(err){console.log('err promise: ',err);})