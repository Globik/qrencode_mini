/**
 * qrencode - QR Code encoder
 *
 * QR Code encoding tool
 * Copyright (C) 2006-2017 Kentaro Fukuchi <kentaro@fukuchi.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#if HAVE_CONFIG_H
# include "config.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#if HAVE_PNG
#include <png.h>
#endif
#include <node_api.h>
#include "qrencode.h"
//#include <unistd.h>
typedef struct{
	int version,micro,margin,size;
	unsigned char fg_color[4];
	const unsigned char* _input;
	QRecLevel level;
	char* _output;
	size_t mem;
	size_t  _bufferlength;
	size_t _out_bufsize;

unsigned char bg_color[4];
	
napi_ref _callback;
napi_async_work _request;
}carrier;

#define INCHES_PER_METER (100.0/2.54)

static int casesensitive = 1;

static int eightbit = 1;//0; harcoded to yes
//static int version = 0;
//static int size = 3;//dot_size=number
//static int margin = -1;// margin=number
static int dpi = 72;
//static int micro = 0;
//static QRecLevel level = QR_ECLEVEL_L;
static QRencodeMode hint = QR_MODE_8;
//static unsigned char fg_color[4] = {0, 0, 0, 255};
//static unsigned char bg_color[4] = {255, 255, 255, 255};

enum imageType {
	PNG_TYPE,
	PNG32_TYPE
};
struct mem_encode{
char* buf;
size_t size;
size_t mem;
};

const int mn=41;
static uint32_t objectLength(napi_env,napi_value);
static const char* getString(napi_env, napi_value);
static bool isNumber(napi_env, napi_value);
static int32_t getZifra(napi_env, napi_value, const char*);
static bool isString(napi_env, napi_value);
static bool hasNamedProperty(napi_env, napi_value, const char*);
static napi_value getNamedProperty(napi_env, napi_value, const char*);

static enum imageType image_type = PNG_TYPE;

static void my_png_write_data(png_structp png_ptr,png_bytep data,png_size_t length){
carrier*c=(carrier*)png_get_io_ptr(png_ptr);
if(!c->_output){
c->_output=(char*)malloc(sizeof(c->_output)*mn);
if(!c->_output){png_error(png_ptr,"malloc png error");printf("malloc c->_output\n");}
c->mem=mn;
}
if(c->_out_bufsize +length > c->mem){
char *new_png=(char*)realloc(c->_output,sizeof(char)*c->_out_bufsize + length);
if(!new_png){ png_error(png_ptr,"realloc png error");printf("realloc png error\n");}
c->_output=new_png;
c->mem+=length;
}
memcpy(c->_output + c->_out_bufsize,data,length);
c->_out_bufsize+=length;
}

static int color_set(unsigned char color[4], const char *value)
{
	//static unsigned char fg_color[4] = {0, 0, 0, 255};
//static unsigned char bg_color[4] = {255, 255, 255, 255};
//color[4]={0,0,0,255};
	int len = strlen(value);
	int i, count;
	unsigned int col[4];
	if(len == 6) {
		count = sscanf(value, "%02x%02x%02x%n", &col[0], &col[1], &col[2], &len);
		if(count < 3 || len != 6) {
			return -1;
		}
		for(i = 0; i < 3; i++) {
			color[i] = col[i];
		}
		printf("*** COLOR[0][1][2]: %02x %02x %02x ***\n",color[0],color[1],color[2]);
		
		color[3] = 255;
		printf("*** color[0][1][2][3]: %d% d% d %d***\n",color[0],color[1],color[2],color[3]);
	} else if(len == 8) {
		count = sscanf(value, "%02x%02x%02x%02x%n", &col[0], &col[1], &col[2], &col[3], &len);
		if(count < 4 || len != 8) {
			return -1;
		}
		for(i = 0; i < 4; i++) {
			color[i] = col[i];
		}
	} else {
		return -1;
	}
	return 0;
}

#if HAVE_PNG
static void fillRow(unsigned char *row, int num, const unsigned char color[])
{
	int i;

	for(i = 0; i < num; i++) {
		memcpy(row, color, 4);
		row += 4;
	}
}
#endif
struct mem_encode  writePNG(const QRcode *qrcode, const char *outfile, enum imageType type,carrier*c)
{
#if HAVE_PNG
	struct mem_encode state;
	
	state.buf=NULL;
	state.size=0;
	state.mem=0;
	
	static FILE *fp; // avoid clobbering by setjmp.
	png_structp png_ptr;
	png_infop info_ptr;
	png_colorp palette = NULL;
	png_byte alpha_values[2];
	unsigned char *row, *p, *q;
	int x, y, xx, yy, bit;
	int realwidth;
	int margin=c->margin;
	int size=c->size;

	realwidth = (qrcode->width + margin * 2) * size;
	if(type == PNG_TYPE) {
		row = (unsigned char *)malloc((size_t)((realwidth + 7) / 8));
	} else if(type == PNG32_TYPE) {
		row = (unsigned char *)malloc((size_t)realwidth * 4);
	} else {
		fprintf(stderr, "Internal error.\n");
		exit(EXIT_FAILURE);
	}
	if(row == NULL) {
		fprintf(stderr, "Failed to allocate memory.\n");
		exit(EXIT_FAILURE);
	}

	if(outfile[0] == '-' && outfile[1] == '\0') {
		fp = stdout;
		
	} else {
		fp = fopen(outfile, "wb");
		if(fp == NULL) {
			fprintf(stderr, "Failed to create file: %s\n", outfile);
			perror(NULL);
			exit(EXIT_FAILURE);
		}
	}

	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(png_ptr == NULL) {
		fprintf(stderr, "Failed to initialize PNG writer.\n");
		exit(EXIT_FAILURE);
	}

	info_ptr = png_create_info_struct(png_ptr);
	if(info_ptr == NULL) {
		fprintf(stderr, "Failed to initialize PNG write.\n");
		exit(EXIT_FAILURE);
	}

	if(setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_write_struct(&png_ptr, &info_ptr);
		fprintf(stderr, "Failed to write PNG image.\n");
		exit(EXIT_FAILURE);
	}

	if(type == PNG_TYPE) {
		palette = (png_colorp) malloc(sizeof(png_color) * 2);
		if(palette == NULL) {
			fprintf(stderr, "Failed to allocate memory.\n");
			exit(EXIT_FAILURE);
		}
		palette[0].red   = c->fg_color[0];
		palette[0].green = c->fg_color[1];
		palette[0].blue  = c->fg_color[2];
		palette[1].red   = c->bg_color[0];
		palette[1].green = c->bg_color[1];
		palette[1].blue  =c-> bg_color[2];
		alpha_values[0] = c->fg_color[3];
		alpha_values[1] = c->bg_color[3];
		png_set_PLTE(png_ptr, info_ptr, palette, 2);
		png_set_tRNS(png_ptr, info_ptr, alpha_values, 2, NULL);
	}

	//png_init_io(png_ptr, fp);
	png_set_write_fn(png_ptr,/*&state*/c, my_png_write_data, NULL);
	if(type == PNG_TYPE) {
		png_set_IHDR(png_ptr, info_ptr,
				(unsigned int)realwidth, (unsigned int)realwidth,
				1,
				PNG_COLOR_TYPE_PALETTE,
				PNG_INTERLACE_NONE,
				PNG_COMPRESSION_TYPE_DEFAULT,
				PNG_FILTER_TYPE_DEFAULT);
	} else {
		png_set_IHDR(png_ptr, info_ptr,
				(unsigned int)realwidth, (unsigned int)realwidth,
				8,
				PNG_COLOR_TYPE_RGB_ALPHA,
				PNG_INTERLACE_NONE,
				PNG_COMPRESSION_TYPE_DEFAULT,
				PNG_FILTER_TYPE_DEFAULT);
	}
	png_set_pHYs(png_ptr, info_ptr,
			dpi * INCHES_PER_METER,
			dpi * INCHES_PER_METER,
			PNG_RESOLUTION_METER);
	png_write_info(png_ptr, info_ptr);

	if(type == PNG_TYPE) {
	/* top margin */
		memset(row, 0xff, (size_t)((realwidth + 7) / 8));
		for(y = 0; y < margin * size; y++) {
			png_write_row(png_ptr, row);
		}

		/* data */
		p = qrcode->data;
		for(y = 0; y < qrcode->width; y++) {
			memset(row, 0xff, (size_t)((realwidth + 7) / 8));
			q = row;
			q += margin * size / 8;
			bit = 7 - (margin * size % 8);
			for(x = 0; x < qrcode->width; x++) {
				for(xx = 0; xx < size; xx++) {
					*q ^= (*p & 1) << bit;
					bit--;
					if(bit < 0) {
						q++;
						bit = 7;
					}
				}
				p++;
			}
			for(yy = 0; yy < size; yy++) {
				png_write_row(png_ptr, row);
			}
		}
		/* bottom margin */
		memset(row, 0xff, (size_t)((realwidth + 7) / 8));
		for(y = 0; y < margin * size; y++) {
			png_write_row(png_ptr, row);
		}
	} else {
	/* top margin */
		fillRow(row, realwidth, c->bg_color);
		for(y = 0; y < margin * size; y++) {
			png_write_row(png_ptr, row);
		}

		/* data */
		p = qrcode->data;
		for(y = 0; y < qrcode->width; y++) {
			fillRow(row, realwidth, c->bg_color);
			for(x = 0; x < qrcode->width; x++) {
				for(xx = 0; xx < size; xx++) {
					if(*p & 1) {
						memcpy(&row[((margin + x) * size + xx) * 4], c->fg_color, 4);
					}
				}
				p++;
			}
			for(yy = 0; yy < size; yy++) {
				png_write_row(png_ptr, row);
			}
		}
		/* bottom margin */
		fillRow(row, realwidth, c->bg_color);
		for(y = 0; y < margin * size; y++) {
			png_write_row(png_ptr, row);
		}
	}

	png_write_end(png_ptr, info_ptr);
	
	png_destroy_write_struct(&png_ptr, &info_ptr);
	fclose(fp);
	free(row);
	free(palette);
return state;
#else
	fputs("\n\nPNG output is disabled at compile time. No output generated.\n", stderr);
	return 0;
#endif
}
static QRcode *encode(const unsigned char *intext, int length,carrier*c)
{
QRcode *code;
if(c->micro) {
if(eightbit) {
// version 3 or 4
//code=QRcode_encodeDataMQR(4,(unsigned char*)"mama\0",4,QR_ECLEVEL_M);
code = QRcode_encodeDataMQR(length, intext, c->version, /*QR_ECLEVEL_M*/c->level);
} else {
code = QRcode_encodeStringMQR((char *)intext, c->version, c->level, hint, casesensitive);
		}
	} else if(eightbit) {
		code = QRcode_encodeData(length, intext, c->version, c->level);
	} else {
		code = QRcode_encodeString((char *)intext, c->version, c->level, hint, casesensitive);
	}

	return code;
}
static struct mem_encode qrencoded(const unsigned char *intext, int length, const char *outfile,carrier*c)

{
	QRcode *qrcode;
	qrcode = encode(intext, length,c);
	if(qrcode == NULL) {
		if(errno == ERANGE) {
			fprintf(stderr, "Failed to encode the input data: Input data too large\n");
		} else {
			perror("Failed to encode the input data");
		}
		exit(EXIT_FAILURE);
	}

	struct mem_encode p;

	switch(image_type) {
		case PNG_TYPE:
		case PNG32_TYPE:
			p=writePNG(qrcode, outfile, image_type,c);
			break;
		default:
			fprintf(stderr, "Unknown image type.\n");
			exit(EXIT_FAILURE);
	}
QRcode_free(qrcode);
	return p;
}


int labuda=0;
void Execute(napi_env env,void* data){
if(data==NULL)return;
 carrier* c=(carrier*)data;
if(c==NULL){fprintf(stderr,"NULL! in malloc\n");return;}
struct mem_encode p=qrencoded(c->_input,c->_bufferlength,"-", c);
//c->_output=(char*)malloc(sizeof(c->_output)*p.size);
//if(c->_output==NULL){fprintf(stderr,"some malloc error\n");return;}
//memcpy(c->_output,p.buf,p.size);
//c->_out_bufsize=p.size;
printf("p->size: %d\n",p.size);
labuda=1;

if(labuda==1){
//free(p.buf);
//p.buf=NULL;
//p.size=0;
labuda=0;
}}

void Complete(napi_env env,napi_status status, void* data){
if(data==NULL)return;
carrier* c=(carrier*)data;
if(status !=napi_ok){
//napi_throw_type_error(env,NULL,"execute callback failed.");
	free(c->_output);
	free(c);
return;}
//napi_handle_scope scope;
	napi_value argv[2];
	//status=napi_open_handle_scope(env,&scope);
	napi_get_null(env,&argv[0]);
status=napi_create_buffer_copy(env,c->_out_bufsize,c->_output,NULL,&argv[1]);	
//if(status !=napi_ok){napi_throw_type_error(env,NULL,"Copy buffer failed. Provide me a buffer.");return;}
	napi_value callback;
	status=napi_get_reference_value(env,c->_callback,&callback);
	napi_value global;
	status=napi_get_global(env,&global);
	//if(status !=napi_ok){napi_throw_type_error(env,NULL,"Get global failed.");return;}
	napi_value result;
	status=napi_call_function(env,global,callback,2,argv,&result);
	//if(status !=napi_ok){napi_throw_type_error(env,NULL,"call_function failed.");return;}
	status=napi_delete_reference(env,c->_callback);
	//if(status !=napi_ok){napi_throw_type_error(env,NULL,"delete reference failed.");return;}
	status=napi_delete_async_work(env,c->_request);
	//if(status !=napi_ok){napi_throw_type_error(env,NULL,"delete_async_work failed.");return;}
	//status=napi_close_handle_scope(env,scope);
	if(status !=napi_ok){
		free(c->_output);
		free(c);
	napi_value argc[2];
	status=napi_create_string_utf8(env,"blabla1.",NAPI_AUTO_LENGTH,&argc[0]);
	if(status !=napi_ok){return;}
	//napi_value argi[2];
	//napi_value cbi=argv[2];
	status=napi_call_function(env,global,argv[1],2, argc,NULL);
	//?	
	}
	free(c->_output);
	c->_output=NULL;
	c->mem=0;c->_out_bufsize=0;
	free(c);
}

napi_value qrencode(napi_env env,napi_callback_info info){
size_t argc=3;
//if(margin <= 0){if(micro){margin=2;}else{margin=4;}}
	napi_status status;
	napi_value argv[3];
	//napi_value arg
	napi_value _this;
	napi_value resource_name;
	void* data;
	carrier* c=(carrier*)malloc(sizeof(carrier));
	if(c==NULL){
		//napi_throw_type_error(env,NULL,"malloc failed.");return NULL;
	return NULL;
	}
	
	status=napi_get_cb_info(env,info,&argc,argv,&_this,&data);
	
	if(status !=napi_ok){
		//napi_throw_type_error(env,NULL,"Get global failed.");return NULL;
		goto brr;
	}
	//unsigned char fg_color[4] = {0, 0, 0, 255};
	c->_output=NULL;
	c->_out_bufsize=0;
	c->mem=0;
	c->version=0;
	c->micro=0;
	c->margin=-1;
	c->size=3;
	c->level = QR_ECLEVEL_L;
	//QRecLevel level = QR_ECLEVEL_L;
	
	//object for options, second argument
	napi_valuetype vtype;
	status=napi_typeof(env,argv[1],&vtype);
	if(status !=napi_ok){napi_throw_type_error(env,NULL,"typeof failed.");return NULL;}
	if(vtype != napi_object){
	napi_throw_type_error(env,NULL,"Wrong type of arguments! Expects an object as second argument.");
	return NULL;
	}else{
	printf("IS NAPI OBJECT FOR OPTIONS! OK!\n");
	napi_value obj=argv[1];
	uint32_t lind;
	napi_value props_names;
	status=napi_get_property_names(env,obj,&props_names);
	if(status !=napi_ok){
	napi_throw_type_error(env,NULL,"get_named_property failed.");return NULL;}
	lind=objectLength(env,props_names);
	if(lind==0){
	napi_throw_type_error(env,NULL,"A provided object must not to be empty!");
	return NULL;}
	
	
	
	//margin
	const char*margi="margin";
	bool has_p;
	has_p=hasNamedProperty(env,obj,margi);
	if(has_p){
	napi_value vresi;
	vresi=getNamedProperty(env,obj,margi);
	int32_t dummyTwo=getZifra(env,vresi," for margin.");
	if(dummyTwo < 0){napi_throw_type_error(env,NULL,"Invalid margin!");return NULL;}
	printf("*** MARGIN: %d ***\n",dummyTwo);
	c->margin=dummyTwo;
	}else{
	printf("*** IN OBJECT NO MARGIN FOUND! ***\n");
	if(c->margin < 0){
	if(c->micro){c->margin=2;}else{c->margin=4;}
	}}
	
	//dot_size , default 3 
	
	bool has_dot_size;
	const char*dotsizi="dot_size";
	has_dot_size=hasNamedProperty(env,obj,dotsizi);
	if(has_dot_size){
	napi_value nres;
	nres=getNamedProperty(env, obj, dotsizi);
	int32_t dotSize=getZifra(env, nres, " for dot_size.");
	if(dotSize <=0){
	napi_throw_type_error(env,NULL,"Invalid dot_size.");
	return NULL;
	}
	c->size=dotSize;
	printf("*** DOT_SIZE: %d ***\n",dotSize);
	}
	
	//micro
	
		
	bool has_micro;
	const char*micri="micro";
	has_micro=hasNamedProperty(env,obj,micri);
	if(has_micro){
	napi_value nres2;
	nres2=getNamedProperty(env, obj, micri);
	int32_t micron=getZifra(env, nres2, " for micro.");
	printf("*** MICRO: %d ***\n", micron);
	if(micron == 1){
	c->micro=micron;
	}else{
	c->micro=0;
	}

	}
	
	
	//version
	
	
	
	
	bool has_version;
	const char*versi="version";
	has_version=hasNamedProperty(env,obj,versi);
	if(has_version){
	napi_value nres3;
	nres3=getNamedProperty(env, obj, versi);
	int32_t verson=getZifra(env, nres3, " for version.");
		if(verson < 0){
		status=napi_throw_type_error(env,NULL,"Invalid version.");
			return NULL;
		}
	if(c->micro && verson > MQRSPEC_VERSION_MAX){
		//fprintf(stderr,"why %d\n",MQRSPEC_VERSION_MAX);
	status=napi_throw_type_error(env,NULL,"Version should be less or equal to 4");// MQRSPEC_VERSION_MAX;
	
	return NULL;
	}else if(!c->micro && verson > QRSPEC_VERSION_MAX){
	status=napi_throw_type_error(env,NULL,"Version should be less or equal to 4");
	return NULL;	
	}
	c->version=verson;
	printf("*** VERSION: %d ***\n",verson);
	if(c->micro){
	if(c->version==0 || c->version==1 || c->version==2){
	napi_throw_type_error(env,NULL,"Version must be specified to encode a Micro QR Code symbol.");
	return NULL;
	}
	}
	}
	
	
	
	
	
	
	
	//level
	
	
	bool has_level;
	const char*leveli="level";
	has_level=hasNamedProperty(env,obj,leveli);
	if(has_level){
	napi_value nres4;
	nres4=getNamedProperty(env, obj, leveli);
	const char*lev;
	lev=getString(env,nres4);
	printf("*** LEVEL: %s ***\n",lev);
	if(strcmp(lev,"h")==0  || strcmp(lev,"H")==0){
	c->level=QR_ECLEVEL_H;
	}else if(strcmp(lev,"m")==0  || strcmp(lev,"M")==0){
	c->level=QR_ECLEVEL_M;
	}else if(strcmp(lev,"l")==0 || strcmp(lev,"L")==0){
	c->level=QR_ECLEVEL_L;
	}else if(strcmp(lev,"q")==0 || strcmp(lev,"Q")==0){
	c->level=QR_ECLEVEL_Q;
	}else{
	napi_throw_type_error(env,NULL,"Invalid level.");
	return NULL;
	}
	}
	
	bool has_b;const char*coli_b="white";
	const char*backi="background_color";
	has_b=hasNamedProperty(env,obj,backi);
	if(has_b){
	napi_value nres5;
	nres5=getNamedProperty(env, obj, backi);
	coli_b=getString(env,nres5);
	printf("BACKGROUND_COLOR: %s\n",coli_b);
	if(color_set(c->bg_color,coli_b)){
	napi_throw_type_error(env,NULL,"Invalid background color value.");
	return NULL;
	}
	}
	
	// colors f
	
	bool has_f;
	const char*fori="foreground_color";
	const char*coli_f="Black";
	has_f=hasNamedProperty(env,obj,fori);
	if(has_f){
	napi_value nres6;
	nres6=getNamedProperty(env, obj, fori);
	coli_f=getString(env,nres6);
	printf("FOREGROUND_COLOR: %s\n",coli_f);
	if(color_set(c->fg_color,coli_f))
	{
	napi_throw_type_error(env,NULL,"Invalid foreground color value.");
	return NULL;
	}
	}
	
	
	
	
	
	}//end options
	
	
	
	
	
	
	
	
	
	
	
	bool isbuf;
	//napi_valuetype isbuf;
	status=napi_is_buffer(env,argv[0],&isbuf);
	if(status !=napi_ok){
		//napi_throw_type_error(env,NULL,"typeof failed.");return NULL;
	goto brr;
	}
	if(isbuf == 0){
		//fprintf(stderr,"BUFFER IS NOT\n");
	napi_value global;
	status=napi_get_global(env,&global);
	if(status !=napi_ok){
		//napi_throw_type_error(env,NULL,"Get global failed.");return NULL;
	goto brr;
	}
	//napi_throw_type_error(env,NULL,"Wrong type of argument! Expects a buffer.");
	//napi_value result3;
	napi_value argc[2];
	status=napi_create_string_utf8(env,"Wrong type of argument! Expects a buffer.",NAPI_AUTO_LENGTH,&argc[0]);
	if(status !=napi_ok){
		//napi_throw_type_error(env,NULL,"create_string failed.");return NULL;
	goto brr;
	}
	//napi_value argi[2];
	napi_value cbi=argv[2];
	status=napi_call_function(env,global,cbi,1, argc,/*&result3*/NULL);
	if(status !=napi_ok) goto brr;
//napi_call_function(env,global,third_argument of javascript aka cb function,args ...(err,result)=>{},err[0] or result[1],can be NULL );
	free(c);
		return NULL;
	}
status=napi_get_buffer_info(env, argv[0],(void**)(&c->_input),&c->_bufferlength);
	if(status !=napi_ok){
		//napi_throw_type_error(env,NULL,"Get buffer_info failed.");return NULL;
	goto brr;
	}
	status=napi_create_reference(env,argv[2],1,&c->_callback);
	if(status !=napi_ok){
	//napi_throw_type_error(env,NULL,"create_reference failed.");return NULL;
	
	}
	status=napi_create_string_utf8(env,"TestResource",NAPI_AUTO_LENGTH,&resource_name);
	if(status !=napi_ok){
		//napi_throw_type_error(env,NULL,"create string failed.");return NULL;
	goto brr;
	}
	status=napi_create_async_work(env,argv[1],resource_name,Execute,Complete,c,&c->_request);
	if(status !=napi_ok){
		//napi_throw_type_error(env,NULL,"create_async_work failed.");return NULL;
	goto brr;
	}
	status=napi_queue_async_work(env,c->_request);
	if(status !=napi_ok){
	//napi_throw_type_error(env,NULL,"queue_async_work failed.");return NULL;
	goto brr;
	}
	return NULL;
	if(0){
	brr:
	fprintf(stderr,"Some error with napi_status in qrencode method\n");
	free(c);
	return NULL;
	}
	}

napi_value setOptions(napi_env env,napi_callback_info info){
	return NULL;
	/*
size_t argc=1;
	napi_value args[1];
	napi_status status;
	status=napi_get_cb_info(env,info,&argc,args,NULL,NULL);
	if(status !=napi_ok){napi_throw_type_error(env,NULL,"Get cb_info failed.");return NULL;}
	if(argc >1){
	status=napi_throw_type_error(env,NULL,"Wrong number of arguments! Must be 1.");
	return NULL;}
	napi_valuetype vtype;
	status=napi_typeof(env,args[0],&vtype);
	if(status !=napi_ok){napi_throw_type_error(env,NULL,"typeof failed.");return NULL;}
	if(vtype != napi_object){
	napi_throw_type_error(env,NULL,"Wrong type of arguments! Expects an object as first argument.");
	return NULL;
	}
	napi_value obj=args[0];
	uint32_t lind;
	napi_value props_names;
	status=napi_get_property_names(env,obj,&props_names);
	if(status !=napi_ok){
	napi_throw_type_error(env,NULL,"get_named_property failed.");return NULL;}
	lind=objectLength(env,props_names);
	if(lind==0){
	napi_throw_type_error(env,NULL,"A provided object must not to be empty!");
	return NULL;}
	const char*margi="margin";
	bool has_p;
	has_p=hasNamedProperty(env,obj,margi);
	if(has_p){
	napi_value vresi;
	vresi=getNamedProperty(env,obj,margi);
	int32_t dummyTwo=getZifra(env,vresi," for margin.");
	if(dummyTwo < 0){
	napi_throw_type_error(env,NULL,"Invalid margin!");
	return NULL;
	}
		
	margin=dummyTwo;
	}else{
	if(margin < 0){
	if(micro){margin=2;}else{margin=4;}
	}
	}
	
	
	bool has_dot_size;
	const char*dotsizi="dot_size";
	has_dot_size=hasNamedProperty(env,obj,dotsizi);
	if(has_dot_size){
	napi_value nres;
	nres=getNamedProperty(env, obj, dotsizi);
	int32_t dotSize=getZifra(env, nres, " for dot_size.");
	if(dotSize <=0){
	napi_throw_type_error(env,NULL,"Invalid dot_size.");
	return NULL;
	}
	size=dotSize;
	}
	
	
	
	
	bool has_micro;
	const char*micri="micro";
	has_micro=hasNamedProperty(env,obj,micri);
	if(has_micro){
	napi_value nres2;
	nres2=getNamedProperty(env, obj, micri);
	int32_t micron=getZifra(env, nres2, " for micro.");
	if(micron == 1){micro=micron;}else{micro=0;}

	}
	
	
	
	
	//version
	bool has_version;
	const char*versi="version";
	has_version=hasNamedProperty(env,obj,versi);
	if(has_version){
	napi_value nres3;
	nres3=getNamedProperty(env, obj, versi);
	int32_t verson=getZifra(env, nres3, " for version.");
		if(verson < 0){
		status=napi_throw_type_error(env,NULL,"Invalid version.");
			return NULL;
		}
	if(micro && verson > MQRSPEC_VERSION_MAX){
		//fprintf(stderr,"why %d\n",MQRSPEC_VERSION_MAX);
	status=napi_throw_type_error(env,NULL,"Version should be less or equal to 4");// MQRSPEC_VERSION_MAX;
	
	return NULL;
	}else if(!micro && verson > QRSPEC_VERSION_MAX){
	status=napi_throw_type_error(env,NULL,"Version should be less or equal to 4");
	return NULL;	
	}
	version=verson;
	if(micro){
	if(version==0 || version==1 || version==2){
	napi_throw_type_error(env,NULL,"Version must be specified to encode a Micro QR Code symbol.");
	return NULL;
	}
	}
	}
	
	
	
	
	
	
	
	//level
	bool has_level;
	const char*leveli="level";
	has_level=hasNamedProperty(env,obj,leveli);
	if(has_level){
	napi_value nres4;
	nres4=getNamedProperty(env, obj, leveli);
	const char*lev;
	lev=getString(env,nres4);
	if(strcmp(lev,"h")==0  || strcmp(lev,"H")==0){
	level=QR_ECLEVEL_H;
	}else if(strcmp(lev,"m")==0  || strcmp(lev,"M")==0){
	level=QR_ECLEVEL_M;
	}else if(strcmp(lev,"l")==0 || strcmp(lev,"L")==0){
	level=QR_ECLEVEL_L;
	}else if(strcmp(lev,"q")==0 || strcmp(lev,"Q")==0){
	level=QR_ECLEVEL_Q;
	}else{
	napi_throw_type_error(env,NULL,"Invalid level.");
	return NULL;
	}
	}
	
	//colors
	bool has_b;const char*coli_b="white";
	const char*backi="background_color";
	has_b=hasNamedProperty(env,obj,backi);
	if(has_b){
	napi_value nres5;
	nres5=getNamedProperty(env, obj, backi);
	coli_b=getString(env,nres5);
	if(color_set(bg_color,coli_b)){
	napi_throw_type_error(env,NULL,"Invalid background color value.");
	return NULL;
		}
	}
	
	// colors f
	
	bool has_f;
	const char*fori="foreground_color";const char*coli_f="Black";
	has_f=hasNamedProperty(env,obj,fori);
	if(has_f){
	napi_value nres6;
	nres6=getNamedProperty(env, obj, fori);
	coli_f=getString(env,nres6);
	if(color_set(fg_color,coli_f)){
	napi_throw_type_error(env,NULL,"Invalid foreground color value.");
	return NULL;
	}
	}
	
	napi_value qr_version,qr_margin,qr_level, qr_size,qr_versi,qr_micro,qr_background_color,qr_foreground_color,qr_full_version;
	napi_value object;

const char*props[]={"full_version","copyright","margin","level","dot_size","micro","version","background_color","foreground_color",NULL};

	//background_color:'76eec6',foreground_color:'ff0000'
	status=napi_create_string_utf8(env,"Copyright (C) 2006-2017 Kentaro Fukuchi.",NAPI_AUTO_LENGTH,&qr_version);
	if(status !=napi_ok){return NULL;}
	status=napi_create_string_utf8(env,coli_b,NAPI_AUTO_LENGTH,&qr_background_color);
	if(status !=napi_ok){return NULL;}
	status=napi_create_string_utf8(env,coli_f,NAPI_AUTO_LENGTH,&qr_foreground_color);
	if(status !=napi_ok){return NULL;}
	status=napi_create_string_utf8(env,QRcode_APIVersionString(),NAPI_AUTO_LENGTH,&qr_full_version);
	if(status !=napi_ok){return NULL;}
	//5
	status=napi_create_int32(env,level,&qr_level);
	if(status !=napi_ok){return NULL;}
	status=napi_create_int32(env,margin,&qr_margin);
	if(status !=napi_ok){return NULL;}
	status=napi_create_int32(env,size,&qr_size);
	if(status !=napi_ok){return NULL;}
	status=napi_create_int32(env,version,&qr_versi);
	if(status !=napi_ok){return NULL;}
	status=napi_create_int32(env,micro,&qr_micro);
	if(status !=napi_ok){return NULL;}
	status=napi_create_object(env,&object);
	if(status !=napi_ok){return NULL;}
	
napi_value jsvalues[]={qr_full_version,qr_version,qr_margin,
* qr_level,qr_size,qr_micro,qr_versi,qr_background_color,qr_foreground_color,NULL};
	
	size_t mf=0;
	while(props[mf]){
	status=napi_set_named_property(env,object,props[mf],jsvalues[mf]);
	if(status !=napi_ok) {return NULL;}
	mf++;
	}
	return object;
	*/
}

napi_value Init(napi_env env,napi_value exports){
	napi_status status;
	napi_value fn1;// fn;
	/*
napi_property_descriptor properties[]={
	{"qrencode",0,qrencode,0,0,0,napi_default,0},
	{"setOptions",0,setOptions,0,0,0,napi_default,0}
};*/
//status=napi_define_properties(env,exports, sizeof(properties)/sizeof(*properties),properties);
//if(status !=napi_ok){napi_throw_type_error(env,NULL,"define_properties failed.");return NULL;}
/*
status=napi_create_function(env,NULL,0,setOptions,NULL,&fn);
if(status !=napi_ok){printf("create function setOtions failed\n");return NULL;}
status=napi_set_named_property(env,exports,"setOptions",fn);
if(status !=napi_ok){printf("set named property failed for setOptions\n"); return NULL;}
*/ 

status=napi_create_function(env,NULL,0,qrencode,NULL,&fn1);
if(status !=napi_ok){printf("create function qrencode failed\n");return NULL;}
status=napi_set_named_property(env,exports,"qrencode",fn1);
if(status !=napi_ok){printf("set named property failed for qrencode\n"); return NULL;}

return exports;
}
NAPI_MODULE(addon,Init)
	
	
uint32_t objectLength(napi_env env, napi_value prop_names){
napi_status status;
uint32_t lindex;
status=napi_get_array_length(env,prop_names,&lindex);
if(status !=napi_ok){napi_throw_type_error(env,NULL,"get_array_length failed.");return 0;}
return lindex;
}
const char* getString(napi_env env,napi_value js_str){
char st[50];
size_t st_size=50;
napi_status status;
if(isString(env,js_str)){
status=napi_get_value_string_utf8(env,js_str,st,st_size,NULL);
if(status !=napi_ok){return NULL;}
const char*du=st;
return du;
}
return NULL;
}
bool isNumber(napi_env env,napi_value nnumber){
napi_status status;
napi_valuetype t;
status=napi_typeof(env,nnumber,&t);
if(status !=napi_ok){return NULL;}
if(t == napi_number) return 1;
return 0;
}

int32_t getZifra(napi_env env,napi_value nresult,const char*er){
int32_t zfr;
char foo[100];
char*f="Expected a number";
napi_status status;
if(!isNumber(env,nresult)){
strcpy(foo,f);
if(er) {strcat(foo,er);}
napi_throw_type_error(env,NULL,foo);
}
status=napi_get_value_int32(env,nresult,&zfr);
if(status !=napi_ok){napi_throw_type_error(env,NULL,"hm");}
return zfr;
}


bool isString(napi_env env,napi_value str){
napi_status status;
napi_valuetype vtype;
status=napi_typeof(env,str,&vtype);
if(status !=napi_ok){return NULL;}
if(vtype == napi_string) return 1;
return 0;
}

bool hasNamedProperty(napi_env env,napi_value obj,const char*s){
bool hasp;
napi_status status;
status=napi_has_named_property(env,obj,s,&hasp);//1=YES! 0=NO!
//if(status !=napi_ok){napi_throw_type_error(env,NULL,"set_named_property failed.");return NULL;}
if(status !=napi_ok) return 0;
return hasp;
}

napi_value getNamedProperty(napi_env env,napi_value obj,const char*s){
napi_value vresi;napi_status status;
status=napi_get_named_property(env,obj,s,&vresi);// displays the value of key "suka" to the vresi
if(status !=napi_ok){return NULL;}
return vresi;
}
