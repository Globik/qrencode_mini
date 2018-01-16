const addon = require('../build/Release/qrencode_mini');
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
console.log("Testing a setOptions({micro:2},'fake'). Should failed.");
try{
addon.setOptions({version:2},'fake');

}catch(e){console.log('Failed!: ',e.name,' ',e.message);a++;}
t=1;

console.log("Testing addon.setOptions({}). Empty object as first arguments should failed.");
try{
addon.setOptions({});
}catch(e){console.log('Failed!: ',e.name,' ', e.message);a++;}

console.log("Testing addon.setOptions(). Should failed.");
t=2;
try{
addon.setOptions();
}catch(e){console.log('Failed!: ',e.name,' ',e.message);a++;}

t=3;
console.log("Testing the version='2'. Must be a number. Should failed if a string.")
try{
addon.setOptions({version:'2'});
}catch(e){console.log('Failed!: ',e.name,' ',e.message);a++;}
t=4;

console.log("Testing a vesrsion=2. Must be a number. Should be be OK.")
try{
let b=addon.setOptions({version:2});
	console.log("OK! Version is: ",b.version);a++;
}catch(e){console.log('Failed!: ',e.name,' ',e.message);}
t=5;

console.log("Testing a micro='2'. Must be a number. Should failed if a string.")
try{
addon.setOptions({micro:'2'});
}catch(e){console.log('Failed!: ',e.name,' ',e.message);a++;}
t=6;

console.log("Testing a micro=1. Must be a number 1 or 0. Should be OK and equal=1")
try{
let b=addon.setOptions({micro:1});
console.log("OK: micro = ",b.micro);a++;
}catch(e){console.log('Failed!: ',e.name,' ',e.message);}
//margin
t=7;
console.log("Testing a margin='2'. Must be a number. Should failed if a string.")
try{
addon.setOptions({margin:'2'});
}catch(e){console.log('Failed!: ',e.name,' ',e.message);a++;}
t=8;

console.log("Testing a margin=2. Must be a number. Should be OK.")
try{
let b=addon.setOptions({margin:2});
	console.log("OK: margin = ",b.margin);a++;
}catch(e){console.log('Failed!: ',e.name,' ',e.message);}
t=9;
//dot_size
console.log("Testing a dot_size='2'. Must be a number. Should failed if a string.")
try{
addon.setOptions({dot_size:'2'});
}catch(e){console.log('Failed!: ',e.name,' ',e.message);a++;}
t=10;

console.log("Testing a dot_size=2. Must be a number. Should be OK.")
try{
let b=addon.setOptions({dot_size:2});
	console.log("OK! dot_size =", b.dot_size);a++;
}catch(e){console.log('Failed!: ',e.name,' ',e.message);}
t=11;
// QR level
console.log("Testing a level='2'. Must be a string. Should failed. Unknown level.")
try{
addon.setOptions({level:'2'});
}catch(e){console.log('Failed!: ',e.name,' ',e.message);a++;}
t=12;

console.log("Testing a level='q'. Must be a string. Should be OK.")
try{
let b=addon.setOptions({level:'q'});
	console.log("OK! Level is",b.level);a++;
}catch(e){console.log('Failed!: ',e.name,' ',e.message);}
t=13;
//QR background_color

console.log("Testing the background_color='2'. Must be a hex string. Should failed if not.")
try{
addon.setOptions({background_color:'2'});
}catch(e){console.log('Failed!: ',e.name,' ',e.message);a++;}
t=14;
console.log("Testing the background_color='ff0000'. Must be a hex string. Should be OK.")
try{
let b=addon.setOptions({background_color:'76eec6'});
console.log("OK! background_color is ",b.background_color);a++;
}catch(e){console.log('Failed!: ',e.name,' ',e.message);}
t=15;
//QR foreground_color
console.log("Testing the foreground_color='2'. Must be a hex string. Should failed if not.")
try{
addon.setOptions({foreground_color:'2'});
}catch(e){console.log('Failed!: ',e.name,' ',e.message);a++;}
t=16;

console.log("Testing the foreground_color='ff0000'. Must be a hex string. Should be OK.")
//t=17;
try{
let b=addon.setOptions({foreground_color:'ff0000'});
console.log("OK! foreground_color is ",b.foreground_color);a++;
}catch(e){console.log('Failed!: ',e.name,' ',e.message);}
t=17;
//testint all available options
console.log("TESTING ALL AVAILABLE OPTIONS");
console.log("OPTIONS: INPUTS:",ob);
console.log("Should be OK");
try{
let b=addon.setOptions(ob);
	console.log("OK! OUTPUTS:",b);a++;
	//assertDeepEqual blablabla
}catch(e){
console.log("Failed!",e.name,e.message);
}
t=18;
console.log("TESTING addon.qrencode(Buffer.from(string))");
console.log("Type String should be failed.");
try{
addon.qrencode(str,{type:'fake'},function(er,data){
	if(er){
	console.log("Failed: ",er,"data: ",data);a++;return;}
	});
}catch(e){
console.log("Failed! In catch",e.name,e.message);
}
t=19;
console.log("Testing type buffer. Should be OK.");
addon.qrencode(buf,{type:'fake'},function(er,data){
if(er){return;}else{a++;console.log("OK! data: ",data.toString('base64').substring(0,16));
				   //console.log('tt:',t);
console.log("total: ",t);
console.log("success: ",a);
					//gu();
}
})
t=20;

function gu(){console.log('su');}



































