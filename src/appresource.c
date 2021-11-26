/*$
 Copyright (C) 2013-2021 Azel.

 This file is part of AzPainter.

 AzPainter is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 AzPainter is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
$*/

/***********************************
 * アプリ用リソース管理
 ***********************************/

#include "mlk_gui.h"
#include "mlk_imagelist.h"
#include "mlk_loadimage.h"
#include "mlk_str.h"

#include "appresource.h"

#include "def_config.h"
#include "def_toolbar_btt.h"


//----------------

static AppResource g_app_resource_body;
AppResource *g_app_resource = NULL;

//---------------

/* フィルタ文字列:通常画像 */

static const char *g_openfile_filter_normal =
"Image file (PNG/JPEG/BMP/GIF/TIFF/WebP/PSD)\t*.bmp;*.png;*.gif;*.jpg;*.jpeg;*.tif;*.tiff;*.webp;*.psd\tAll files\t*";

/* mLoadImageCheck:通常画像用 */

static mFuncLoadImageCheck g_func_loadimage_check_normal[] = {
	mLoadImage_checkPNG, mLoadImage_checkJPEG, mLoadImage_checkBMP,
	mLoadImage_checkGIF, mLoadImage_checkTIFF, mLoadImage_checkWEBP,
	mLoadImage_checkPSD, 0
};

/* ツールバーのデフォルトデータ
 * 255 = 終了, 254 = 区切り */

static const uint8_t g_toolbar_def_btts[] = {
	TOOLBAR_BTT_NEW, TOOLBAR_BTT_OPEN, TOOLBAR_BTT_OPEN_RECENT, 254,
	TOOLBAR_BTT_SAVE, TOOLBAR_BTT_SAVE_AS, TOOLBAR_BTT_SAVE_DUP, 254,
	TOOLBAR_BTT_UNDO, TOOLBAR_BTT_REDO, 254,
	TOOLBAR_BTT_CURLAYER_ERASE, TOOLBAR_BTT_SEL_RELEASE, 254,
	TOOLBAR_BTT_VIEW_PANEL, TOOLBAR_BTT_VIEW_CANVAS_MIRROR, TOOLBAR_BTT_VIEW_BKGND_PLAID,
	TOOLBAR_BTT_VIEW_GRID, TOOLBAR_BTT_VIEW_GRID_SPLIT, TOOLBAR_BTT_PANEL_FILTER_LIST, 254,
	TOOLBAR_BTT_OPT_GRID, TOOLBAR_BTT_CANVAS_SCALE,
	255
};

/* 数値用1bitイメージ: "0-9 | - | . | % | 度 | L" [5x9]x15 */

static const uint8_t g_img1bit_number_5x9[]={
0x86,0x18,0x43,0x9e,0x79,0xc6,0x00,0x20,0x45,0x00,0xc9,0xa4,0x64,0x42,0x4a,0x29,
0x01,0x50,0x4b,0x00,0xa9,0xa4,0x64,0x42,0x48,0x29,0x01,0xa0,0x4a,0x00,0x89,0x20,
0x54,0xce,0x49,0x29,0x01,0x80,0x44,0x00,0x89,0x10,0x53,0x52,0x22,0xc6,0x3d,0x40,
0x40,0x00,0x89,0x08,0x54,0x50,0x22,0x09,0x01,0x20,0x40,0x00,0x89,0x84,0xf4,0x50,
0x22,0x89,0x00,0xa0,0x40,0x00,0x89,0x84,0x44,0x52,0x22,0x89,0x00,0x53,0x41,0x00,
0x86,0x3c,0x43,0x8c,0x21,0x46,0x00,0x93,0xc0,0x03 };

//メニューボタン (7x7)
static const uint8_t g_img1bit_menubtt[] = {0x7f,0x00,0x00,0x7f,0x00,0x00,0x7f};

//PNG:リスト編集用アイコン (1bit, 16x16)
static const uint8_t g_png_listicon[] = {
0x89,0x50,0x4e,0x47,0x0d,0x0a,0x1a,0x0a,0x00,0x00,0x00,0x0d,0x49,0x48,0x44,0x52,
0x00,0x00,0x00,0xa0,0x00,0x00,0x00,0x10,0x01,0x00,0x00,0x00,0x00,0xb9,0x7e,0x49,
0xf5,0x00,0x00,0x00,0xd0,0x49,0x44,0x41,0x54,0x18,0x57,0x4d,0xcf,0xb1,0x6a,0xc3,
0x40,0x0c,0x06,0xe0,0xff,0xf0,0x60,0x32,0x79,0x0e,0x04,0xee,0x39,0x0c,0xa5,0xea,
0xa3,0xf4,0x35,0x3a,0x9d,0x02,0x99,0xd3,0x40,0x97,0xd2,0x9a,0xe8,0x55,0x0c,0x2d,
0x74,0xc9,0x43,0x78,0xeb,0xea,0x6e,0x3e,0x38,0xa2,0x9c,0x2e,0xd7,0xb4,0x82,0x1f,
0xc4,0x07,0x42,0x12,0xf4,0x56,0xf0,0xfa,0xa3,0xca,0x2e,0x77,0x89,0x54,0xcf,0x21,
0x11,0x00,0xcf,0x8f,0xdb,0x3f,0x4c,0x64,0x48,0xc2,0x0f,0x50,0x89,0x73,0x28,0xb8,
0xf8,0x44,0xd9,0x45,0x18,0x18,0xfa,0x89,0x0a,0xce,0x5d,0xc6,0x1d,0xb1,0x38,0x40,
0xe2,0xe8,0x0b,0x4e,0x6d,0xc6,0x13,0xf1,0xb1,0x51,0x1d,0xfa,0xaf,0x0e,0xec,0xd8,
0x8d,0x4d,0xc1,0x61,0x68,0x6d,0xfc,0x79,0x5f,0x90,0x5d,0xc5,0x95,0xa1,0x7c,0xe2,
0x4a,0xec,0x0c,0xdf,0xd6,0x86,0x7e,0xac,0x38,0x36,0x86,0xef,0x3d,0x70,0x58,0xdd,
0xa1,0xde,0x39,0xb5,0x74,0xc2,0xe6,0x10,0x81,0xd7,0xf5,0xfd,0x2f,0xce,0x1d,0xed,
0xb0,0x79,0x89,0x36,0x1e,0x5c,0xc5,0xc5,0x53,0x8a,0x49,0x32,0x1e,0x9f,0xc2,0xf6,
0xdf,0x9b,0x57,0x94,0x18,0x3e,0x2a,0x9e,0x03,0xb0,0xe4,0xa5,0xc8,0xd1,0x6f,0xe8,
0xad,0x0c,0x55,0x2d,0x17,0xc1,0x52,0xa7,0xf7,0xea,0xbb,0xba,0xe3,0x00,0x00,0x00,
0x00,0x49,0x45,0x4e,0x44,0xae,0x42,0x60,0x82 };

//PNG:ファイルアイコン (color, [16x16] x3)
static const uint8_t g_png_fileicon[] = {
0x89,0x50,0x4e,0x47,0x0d,0x0a,0x1a,0x0a,0x00,0x00,0x00,0x0d,0x49,0x48,0x44,0x52,
0x00,0x00,0x00,0x30,0x00,0x00,0x00,0x10,0x08,0x03,0x00,0x00,0x00,0x67,0x70,0x0c,
0x83,0x00,0x00,0x00,0x12,0x50,0x4c,0x54,0x45,0x00,0x00,0x01,0xff,0xff,0xff,0x00,
0x00,0x00,0xd9,0xc8,0x58,0xdd,0xdd,0xdd,0xfb,0xf5,0xa6,0xb5,0xbf,0x53,0x37,0x00,
0x00,0x00,0x01,0x74,0x52,0x4e,0x53,0x00,0x40,0xe6,0xd8,0x66,0x00,0x00,0x00,0x70,
0x49,0x44,0x41,0x54,0x78,0x9c,0xdd,0xd1,0x51,0x0e,0x80,0x20,0x0c,0x03,0x50,0x5a,
0xe7,0xfd,0xaf,0x6c,0xd8,0xd8,0x60,0x02,0x51,0x7f,0xed,0x97,0x81,0x3e,0x46,0xb0,
0x14,0x0b,0x50,0x3e,0x05,0xe4,0x56,0xd0,0x93,0xfb,0x21,0xa6,0x4d,0x42,0x43,0x30,
0xf7,0x5d,0xf0,0xac,0xe1,0x02,0x84,0x80,0xcf,0x54,0xc1,0x43,0xd3,0x07,0x75,0x20,
0xcc,0x7d,0x13,0x0d,0x34,0x36,0x00,0x3f,0x60,0xe8,0xab,0xb0,0x2b,0x59,0x46,0x00,
0x11,0x9d,0x90,0xfa,0x55,0x6c,0x41,0xbb,0x52,0x9f,0x67,0x5f,0x8f,0x60,0x7e,0xd6,
0x3f,0x80,0xe9,0x3f,0x48,0x64,0x0d,0x52,0x6e,0x0b,0x01,0xec,0x45,0xdf,0xe4,0x02,
0xff,0x68,0x04,0x50,0x68,0x7b,0x15,0xa8,0x00,0x00,0x00,0x00,0x49,0x45,0x4e,0x44,
0xae,0x42,0x60,0x82 };

//PNG:定規アイコン (1bit, [17x17] x8)
static const uint8_t g_png_ruleicon[263] = {
0x89,0x50,0x4e,0x47,0x0d,0x0a,0x1a,0x0a,0x00,0x00,0x00,0x0d,0x49,0x48,0x44,0x52,
0x00,0x00,0x00,0x88,0x00,0x00,0x00,0x11,0x01,0x00,0x00,0x00,0x00,0x2e,0xa8,0xd9,
0x74,0x00,0x00,0x00,0xce,0x49,0x44,0x41,0x54,0x78,0x5e,0x5d,0x90,0x31,0x6a,0x03,
0x51,0x0c,0x44,0x05,0x69,0xb6,0xcb,0x79,0x7e,0xe5,0xa3,0xe4,0x18,0xc6,0x04,0x4b,
0xe5,0x1e,0xc3,0xb7,0x08,0xb8,0x88,0xc5,0xb2,0xd5,0xaf,0x52,0xa5,0x0c,0x5a,0xd8,
0x3e,0x72,0x9a,0x08,0x5b,0xfc,0x9d,0x14,0x5e,0xbb,0x48,0x3f,0xcc,0x7b,0x33,0x04,
0x00,0xe1,0xc6,0x11,0x31,0x01,0xc0,0x42,0x09,0xc0,0x4d,0x39,0xc3,0x0f,0x1b,0x00,
0x8d,0x2e,0x98,0xbb,0x17,0x15,0x6e,0x61,0x62,0xd3,0xa6,0x4f,0xba,0x62,0xe8,0x84,
0xc1,0x4b,0x9c,0xe0,0x87,0x8f,0x3e,0x28,0xa5,0x96,0xa5,0x4d,0xf4,0xbc,0xdd,0xff,
0x84,0x44,0xef,0x94,0x5a,0x4b,0x4b,0x30,0xde,0xda,0xf7,0x2f,0xb2,0x37,0x4a,0xab,
0xdd,0x6b,0x80,0xf1,0x99,0xb6,0x3b,0xef,0xa1,0x94,0x5e,0xbb,0xad,0x1b,0x63,0x4e,
0x3d,0xfa,0x3b,0x84,0x32,0x6a,0x71,0x53,0x3e,0xd3,0x93,0x1e,0xed,0x6b,0x01,0x65,
0xd6,0x62,0x2a,0xf7,0x4c,0x03,0xe5,0x50,0x0b,0xf8,0xd1,0x93,0xa0,0xd4,0x95,0x7e,
0x63,0xc5,0x48,0x29,0x2b,0xfd,0xe6,0xe3,0x23,0x5d,0x31,0x94,0x8c,0x87,0xb3,0x8d,
0x74,0xc1,0x5c,0xc2,0xed,0xbe,0x4b,0xc7,0xff,0xdb,0x05,0x04,0x00,0xa6,0xb2,0xfe,
0xb3,0x00,0x7f,0xa5,0x41,0xe4,0x40,0x3e,0x6d,0xff,0x3e,0x00,0x00,0x00,0x00,0x49,
0x45,0x4e,0x44,0xae,0x42,0x60,0x82 };

//1bit白黒:オプションパネルのタイプ
static const unsigned char g_img1bit_option_type[]={
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x01,0x00,0x18,0x00,0x80,0x01,
0x00,0x18,0x00,0x80,0x01,0x0f,0x18,0x00,0x80,0xfd,0xff,0x1b,0x00,0xb0,0xf1,0xff,
0x18,0xff,0x9f,0x9d,0x99,0x1b,0x00,0x9e,0x11,0x80,0x18,0x55,0x95,0x9d,0x99,0x1b,
0x80,0x8f,0x11,0x80,0x18,0x55,0x95,0x65,0x66,0x1a,0xe0,0x87,0x11,0x86,0x18,0x55,
0x95,0x65,0x66,0x1a,0xf0,0x83,0xf1,0xff,0x18,0x01,0x90,0x9d,0x99,0x1b,0xfc,0x80,
0x11,0x86,0x18,0x01,0x90,0x9d,0x99,0x1b,0x7e,0x80,0x11,0x80,0x18,0x01,0x90,0x65,
0x66,0x1a,0x0f,0x80,0x11,0x80,0x18,0xff,0x9f,0x65,0x66,0x9a,0x03,0x80,0xf1,0xff,
0x18,0x00,0x80,0xfd,0xff,0x5b,0x00,0x80,0x01,0x00,0x18,0x00,0x80,0x01,0x00,0x18,
0x00,0x80,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff };

//---------------



/* 指定サイズの PNG を読み込み */

static mImageList *_load_imglist_size(const char *prefix,int size)
{
	mStr str = MSTR_INIT;
	mImageList *img;

	mStrSetFormat(&str, "!/img/%s-%d.png", prefix, size);

	img = mImageListLoadPNG(str.buf, size);

	mStrFree(&str);

	return img;
}

/* モノクロ PNG 画像から mImageList 作成 */

static mImageList *_create_imglist_png_mono(const uint8_t *buf,int bufsize,int size)
{
	mImageList *img;
	
	img = mImageListLoadPNG_buf(buf, bufsize, size);

	if(img)
		mImageListReplaceTextColor_mono(img);

	return img;
}


//=======================


/** 解放 */

void AppResource_free(void)
{
	AppResource *p = APPRES;

	if(!p) return;
	
	mImageListFree(p->imglist_icon_other);
	mImageListFree(p->imglist_toolbar);
	mImageListFree(p->imglist_paneltool_tool);
	mImageListFree(p->imglist_paneltool_sub);
	mImageListFree(p->imglist_panelopt_type);
	mImageListFree(p->imglist_panelcolor_type);
	mImageListFree(p->imglist_panelcolpal_type);
	mImageListFree(p->imglist_panellayer_check);
	mImageListFree(p->imglist_panellayer_cmd);
}

/** 初期化 */

void AppResource_init(void)
{
	AppResource *p;

	mMemset0(&g_app_resource_body, sizeof(AppResource));

	p = g_app_resource = &g_app_resource_body;

	//イメージリスト

	p->imglist_icon_other = _load_imglist_size("icon_other", APPCONF->iconsize_other);

	p->imglist_toolbar = _load_imglist_size("icon_toolbar", APPCONF->iconsize_toolbar);

	p->imglist_paneltool_tool = _load_imglist_size("icon_tool", APPCONF->iconsize_panel_tool);

	p->imglist_paneltool_sub = _load_imglist_size("icon_toolsub", APPCONF->iconsize_panel_tool);

	p->imglist_panelopt_type = mImageListCreate_1bit_mono(g_img1bit_option_type, 20 * 4, 14, 20);

	p->imglist_panelcolor_type = mImageListLoadPNG("!/img/coltype-150.png", 27);

	p->imglist_panelcolpal_type = mImageListLoadPNG("!/img/colpal_type-150.png", 30);

	p->imglist_panellayer_check = mImageListLoadPNG("!/img/layer_check-150.png", 20);

	p->imglist_panellayer_cmd = mImageListLoadPNG("!/img/layer_cmd-150.png", 20);
}

/** ファイル開く用フィルタ文字列(通常画像用) 取得*/

const char *AppResource_getOpenFileFilter_normal(void)
{
	return g_openfile_filter_normal;
}

/** mLoadImageCheck 関数の配列 (通常画像用) 取得 */

mFuncEmpty *AppResource_getLoadImageChecks_normal(void)
{
	return (mFuncEmpty *)g_func_loadimage_check_normal;
}

/** ツールバーのデフォルトボタンデータを取得 */

const uint8_t *AppResource_getToolBarDefaultBtts(void)
{
	return g_toolbar_def_btts;
}

/** 数値用1bitイメージ取得 */

const uint8_t *AppResource_get1bitImg_number5x9(void)
{
	return g_img1bit_number_5x9;
}

/** メニューボタン用1bitイメージ取得 */

const uint8_t *AppResource_get1bitImg_menubtt(void)
{
	return g_img1bit_menubtt;
}

/** リスト編集用アイコンのイメージリストを作成 */

mImageList *AppResource_createImageList_listedit(void)
{
	return _create_imglist_png_mono(g_png_listicon, sizeof(g_png_listicon), 16);
}

/** ファイルアイコンのイメージリストを作成 */

mImageList *AppResource_createImageList_fileicon(void)
{
	return mImageListLoadPNG_buf(g_png_fileicon, sizeof(g_png_fileicon), 16);
}

/** 定規アイコンのイメージリストを作成 */

mImageList *AppResource_createImageList_ruleicon(void)
{
	return _create_imglist_png_mono(g_png_ruleicon, sizeof(g_png_ruleicon), 17);
}

