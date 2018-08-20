const qrencode_mini=require('./build/Release/qrencode_mini');

function qrencode(buf,opti){
if(!buf){return;}
if(!opti){console.log("no options provided");return;}
let b;
if(typeof  buf==='string'){
//console.log("yes it's a string");
if(buf.length > 200){
console.log('WARNING: String is too big. Truncate it.');
buf=buf.substring(0,200);//???
}
b=Buffer.from(buf);
}
//else 
if(Buffer.isBuffer(buf)){
//console.log("IS BUFFER!");
b=buf;
}
//else{}
return new Promise(function(res,rej){
qrencode_mini.qrencode(b,opti,function(err,result){
if(err){rej(err);}
res(result);
})
})
}
//module.exports={setOptions,qrencode};
module.exports={qrencode};
