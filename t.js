var d=require('./index.js');
console.log(d);
let m=d.setOptions({margin:3});
console.log('m: ',m);
let str="mama";
let buf=Buffer.from(str);
d.qrencode(buf,{type:'fake'},function(er,da){
console.log(er,da)
})

function su(buf){
return new Promise(function(res,rej){
d.qrencode(buf,{fake:'fake'},function(er,da){
if(er)rej(er)
res(da)
})
})
}
su(buf).then(d=>console.log('dh: ',d));

(async function(){
try{
let a=await su(buf);
console.log('A: ',a);
}catch(e){console.log('hier error: ',e)}
})()