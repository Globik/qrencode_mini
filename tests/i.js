const addon = require('../build/Release/qrencode_mini');
const util=require('util');
var t=0;var a=0;
const ob={
background_color:'76eec6', //optional default white ; a hex string #76eec6
foreground_color:'ff0000', //optional default black ; a hex string  #ff0000
level:"q",   // optional default low="l" ; a string
dot_size:6, //optional default 3 ; a number
margin:2,  //optional default 4 ; a number
micro:0,  //optional 0 = NO 1 = YES ; a number
version:4 //optional ; a number
}
const str="mama";
const buf=Buffer.from(str);
function colorize(color,text){
const codes=util.inspect.colors[color];
	return `\x1b[${codes[0]}m${text}\x1b[${codes[1]}m`;
}
function c(){
let r={};
	Object.keys(util.inspect.colors).forEach(color=>{
	r[color]=text=>colorize(color,text)
	})
	return r;
}
var ew=c();


console.log(ew.bold(ew.blue("encoding string 'mama'")));
addon.qrencode(buf,ob,function(er,data){
if(er){console.log('error: ',er);return;}else{a++;console.log(ew.green("OK! data: "),data.toString('base64').substring(0,160));
	console.log(data.toString());
}
})
