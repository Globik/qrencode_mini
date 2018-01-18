# qrencode_mini


Node.js addon(N-API) QR-code generator based on [libqrencode](https://github.com/fukuchi/libqrencode) in C - a fast and compact QR Code encoding
library.
qrencode_mini module is for node.js server side usage. Implements not all feaures like its original lib has.
Produces a buffer as output. That can be encoded to base64 string and added to the image src attribute in a html murkup.

Micro QR feature is some way experimental at the moment.

# Dependencies

[libpng](https://github.com/glennrp/libpng) must be installed.

# Installation

```
npm install qrencode_mini

```

# Usage

There are only two methods. 

```
const {setOptions, qrencode} = require('qrencode_mini');

// only for the use at very beginning of a file:
const b = setOptions({margin:4}); // synchronous, optional(can be skiped),
//for the options settings which are also optional
console.log("B: ",b); 
// outputs an object with version, copyright and setted options or error info.

// later somewhere in a router after http request:
let str="bitcoin:1BMXmqU3fZ8PVjPbxgeenEX93YYf74bjeB?amount50&label=John&message=Donation%20for%20project";
let buf=Buffer.from(str);

// a promise:

qrencode(buf).then(function(data){
console.log('data: ','data:image/png;base64,'+data.toString('base64'));
}).catch(function(err){console.log('err promise: ',err);
})

```

## setOptions({options})

1. It is synchronous, must be at runtime called when server first time started.

```
const options = {
background_color:'#76eec6', // hex string, optional, default: black
foreground_color:'#ff0000', // hex string, optional, default: white
level:"q", // string, optional, default: "l", LOW, 
//can be:"m", Medium
//       "q", Quartile
//       "h", High
dot_size: 6, // number, optional, default: 3
margin:2,    // number, optional, default: 4
micro:0,     // number, optional, default: 0 
// if a string to be encoded to the microQR? 0 means "NO", 1 = "YES"
version:4    // number, optional, default: 0 
// can be 0, 1, 2, 3, 4 //for microQR(if micro=1) must be equal to 3 or 4!!!!
}

let out = setOptions(options);
console.log(out);

// Must be something like that:
{ full_version: '4.0.0',
  copyright: 'Copyright (C) 2006-2017 Kentaro Fukuchi.',
  margin: 4,
  level: 0,
  dot_size: 3,
  micro: 0,
  version: 0, 
  background_color: 'white',
  foreground_color: 'Black' }
```
The version for microQR if micro = 1, must be specified to 3 or 4!

# QR pics


1. QR encoded "mama"

![alt text](http://gifok.net/images/2018/01/19/qr_not_micro.png)

2. microQR encoded "mama"

![alt text](http://gifok.net/images/2018/01/19/micro_qr.png)


# Perfomance


* qrencode_mini is about x3 times faster than [qr-node](https://github.com/xr0master/qr-node)
* qrencode_mini is about x300 times faster than [qrcode-js](https://github.com/CloudService/qrcode-js)

| Iterations  | 1     | 5     | 100   |
| :---: | :---: | :---: | :---: |
| [qrencode_mini](https://github.com/Globik/qrencode_mini) | 1.871ms   | 5.181ms    | 5.587ms    |
| :---: | :---: | :---: | :---: |
| [qr-node](https://github.com/xrOmaster/qr-node)          | 4.940ms   | 41.030ms   | 519.877ms  |
| :---: | :---: | :---: | :---: |
| [qrcode-js](https://github.com/CloudService/qrcode-js)   | 297.948ms | 1405.711ms | 9803.455ms |
| :---: | :---: | :---: | :---: |
