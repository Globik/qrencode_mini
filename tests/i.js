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
console.log(ew.green("Testing addon.setOptions({}). Empty object as first arguments should failed."));
try{
addon.setOptions({});
}catch(e){console.log(ew.red('Failed!: '),e.name,' ', e.message);a++;}

console.log("Testing addon.setOptions(). Should failed.");
t=1;
try{
addon.setOptions();
}catch(e){console.log(ew.red('Failed!: '),e.name,' ',e.message);a++;}

t=2;
console.log("Testing the version='2'. Must be a number. Should failed if a string.")
try{
addon.setOptions({version:'2'});
}catch(e){console.log(ew.red('Failed!: '),e.name,' ',e.message);a++;}
t=3;

console.log("Testing a vesrsion=2. Must be a number. Should be be OK.")
try{
let b=addon.setOptions({version:2});
	console.log(ew.green("OK! Version is: "),b.version);a++;
}catch(e){console.log('Failed!: ',e.name,' ',e.message);}
t=4;

console.log("Testing a micro='2'. Must be a number. Should failed if a string.")
try{
addon.setOptions({micro:'2'});
}catch(e){console.log(ew.red('Failed!: '),e.name,' ',e.message);a++;}
t=5;

console.log("Testing a micro=1. Must be a number 1 or 0. Should be OK and equal=1")
try{
let b=addon.setOptions({micro:1});
console.log(ew.green("OK: micro = "),b.micro);a++;
}catch(e){console.log('Failed!: ',e.name,' ',e.message);}
//margin
t=6;
console.log("Testing a margin='2'. Must be a number. Should failed if a string.")
try{
addon.setOptions({margin:'2'});
}catch(e){console.log(ew.red('Failed!: '),e.name,' ',e.message);a++;}
t=7;

console.log("Testing a margin=2. Must be a number. Should be OK.")
try{
let b=addon.setOptions({margin:2});
	console.log(ew.green("OK: margin = "),b.margin);a++;
}catch(e){console.log('Failed!: ',e.name,' ',e.message);}
t=8;
//dot_size
console.log("Testing a dot_size='2'. Must be a number. Should failed if a string.")
try{
addon.setOptions({dot_size:'2'});
}catch(e){console.log(ew.red('Failed!: '),e.name,' ',e.message);a++;}
t=9;

console.log("Testing a dot_size=2. Must be a number. Should be OK.")
try{
let b=addon.setOptions({dot_size:2});
	console.log(ew.green("OK! dot_size ="), b.dot_size);a++;
}catch(e){console.log('Failed!: ',e.name,' ',e.message);}
t=10;
// QR level
console.log("Testing a level='2'. Must be a string. Should failed. Unknown level.")
try{
addon.setOptions({level:'2'});
}catch(e){console.log(ew.red('Failed!: '),e.name,' ',e.message);a++;}
t=11;

console.log("Testing a level='q'. Must be a string. Should be OK.")
try{
let b=addon.setOptions({level:'q'});
	console.log(ew.green("OK! Level is"),b.level);a++;
}catch(e){console.log('Failed!: ',e.name,' ',e.message);}
t=12;
//QR background_color

console.log("Testing the background_color='2'. Must be a hex string. Should failed if not.")
try{
addon.setOptions({background_color:'2'});
}catch(e){console.log(ew.red('Failed!: '),e.name,' ',e.message);a++;}
t=13;
console.log("Testing the background_color='ff0000'. Must be a hex string. Should be OK.")
try{
let b=addon.setOptions({background_color:'ff0000'});
console.log(ew.green("OK! background_color is "),b.background_color);a++;
}catch(e){console.log('Failed!: ',e.name,' ',e.message);}
t=14;
//QR foreground_color
console.log("Testing the foreground_color='2'. Must be a hex string. Should failed if not.")
try{
addon.setOptions({foreground_color:'2'});
}catch(e){console.log(ew.red('Failed!: '),e.name,' ',e.message);a++;}
t=15;

console.log("Testing the foreground_color='ff0000'. Must be a hex string. Should be OK.")
//t=17;
try{
let b=addon.setOptions({foreground_color:'ff0000'});
console.log(ew.green("OK! foreground_color is "),b.foreground_color);a++;
}catch(e){console.log('Failed!: ',e.name,' ',e.message);}
t=16;
//testint all available options
console.log("TESTING ALL AVAILABLE OPTIONS");
console.log("OPTIONS: INPUTS:",ob);
console.log("Should be OK");
try{
let b=addon.setOptions(ob);
	console.log(ew.green("OK! OUTPUTS:"),ew.yellow(`${JSON.stringify(b)}`));a++;
	//assertDeepEqual blablabla
}catch(e){
console.log("Failed!",e.name,e.message);
}
t=17;
console.log("TESTING addon.qrencode(Buffer.from(string))");
console.log("Type String should be failed.");
try{
addon.qrencode(str,{type:'fake'},function(er,data){
	if(er){
	console.log(ew.red("Failed: "),ew.underline(ew.red(er)),"data: ",data);a++;return;}
	});
}catch(e){
console.log(ew.red("Failed! In catch"),e.name,e.message);
}
t=18;
console.log(ew.bold(ew.blue("Testing type buffer. Should be OK.")));
addon.qrencode(buf,{type:'fake'},function(er,data){
if(er){return;}else{a++;console.log(ew.green("OK! data: "),data.toString('base64').substring(0,160));
				   //console.log('tt:',t);
console.log(ew.green("total: "),t);
console.log(ew.green("success: "),ew.red(`${a}`));
					//gu();
}
})
t=19;
function gu(){console.log('su');console.log(t,'=',a);}