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

/*******************************
 * PanelLayerList の内部定義
 *******************************/


struct _PanelLayerList
{
	MLK_SCROLLVIEWPAGE_DEF

	LayerItem *item;	//D&D 現在の移動先 (_MOVEDEST_INSERT で NULL の場合は終端へ)
	int fpress,
		dnd_type,		//D&D 現在の移動先のタイプ
		dnd_pos;		//D&D Y位置 (判定中:押し時の位置, D&D中:マーカー表示位置)
	mPoint drag_ptpos;
};

/* マクロ */

#define _PAGE(p)  ((PanelLayerList *)(p))

#define _EACH_H           40	//各レイヤの高さ
#define _MIN_WIDTH        205	//レイヤ一つの最小幅
#define _DEPTH_W          12	//深さの左余白幅
#define _VISIBLE_BOX_SIZE 13	//表示ボックスのサイズ
#define _PREV_H           (_EACH_H - 7)	//プレビューの高さ
#define _PREV_W           _PREV_H
#define _FLAGS_BOX_SIZE   14	//フラグボックスの各サイズ
#define _FLAGS_PAD        2     //フラグボックス間の余白
#define _FLAGS_ALLW       ((_FLAGS_BOX_SIZE + _FLAGS_PAD) * 5 - _FLAGS_PAD)	//フラグ全体の幅

#define _UPPER_Y        4	//上段のY位置
#define _LOWER_Y        (_EACH_H - 1 - 3 - _FLAGS_BOX_SIZE)	//下段のY位置
#define _PREV_X         3	//プレビュー位置
#define _PREV_Y         3
#define _INFO_X         (_PREV_X + _PREV_W + 3)	//表示ボックス、フラグボックスのX位置
#define _INFO_NAME_X    (_INFO_X + _VISIBLE_BOX_SIZE + 4)	//レイヤ名のX位置
#define _FLAGS_X		(_INFO_X + _FLAGS_BOX_SIZE + _FLAGS_PAD)	//フラグの左位置
#define _LAYERTYPE_X    _INFO_X		//レイヤタイプX位置
#define _TONEINFO_X     (_FLAGS_X + _FLAGS_ALLW + 3)  //トーン情報の位置

#define _FOLDER_ARROW_W  9
#define _FOLDER_ICON_X   5
#define _FOLDER_ARROW_X  (_INFO_X - 5 - _FOLDER_ARROW_W)

#define _COL_SEL_BKGND    0xffc2b8  //選択時の背景色
#define _COL_SEL_FRAME    0xff0000  //選択時の枠色
#define _COL_SEL_NAME     0xc80000  //選択時の名前テキスト色
#define _COL_VISIBLE_BOX  0x002080  //表示ボックスの中の色

#define _DRAWONE_F_UPDATE	  1		//直接描画時、範囲を更新 (ウィンドウ転送時)
#define _DRAWONE_F_ONLY_INFO  2		//[背景/プレビュー/レイヤ名] 以外のみ描画

#define _PRESS_MOVE_BEGIN 1
#define _PRESS_MOVE       2

#define _TIMER_SCROLL_TIME  150  //D&D時のスクロールの間隔(ms)

/* フラグボックス */

enum
{
	_FLAGBOX_NO_FILLREF,
	_FLAGBOX_NO_MASKLAYER,
	_FLAGBOX_NO_LOCK,
	_FLAGBOX_NO_CHECK,
	_FLAGBOX_NO_ALPHAMASK,
	_FLAGBOX_NO_MASK_UNDER
};

/* 左ボタン押し時の処理 */

enum
{
	_PRESSLEFT_DONE,			//処理済み (カレント変更なし)
	_PRESSLEFT_SETCURRENT,		//カレントレイヤの変更
	_PRESSLEFT_SETCURRENT_DONE,	//カレントレイヤの変更 (処理をしたあとでカレントも変更する場合)
	_PRESSLEFT_OPTION,			//レイヤ設定を実行
	_PRESSLEFT_SETCOLOR			//線の色設定
};

/* D&D 移動先のタイプ (負の値で移動しない) */

enum
{
	_MOVEDEST_NONE   = -1,	//移動先なし
	_MOVEDEST_SCROLL = -2,	//スクロール中
	_MOVEDEST_INSERT = 0,	//item の上へ移動
	_MOVEDEST_FOLDER = 1	//item のフォルダ内へ移動
};

//--------------

//フラグボックスの各フラグ値
// 0=マスクレイヤ

static const uint8_t g_flagbox_flags[] = {
	LAYERITEM_F_FILLREF, 0, LAYERITEM_F_LOCK, LAYERITEM_F_CHECKED
};

//-----------

//右クリックメニューデータ

static const uint16_t g_menudat_rbtt[] = {
	TRMENU_LAYER_NEW, TRMENU_LAYER_NEW_ABOVE, MMENU_ARRAY16_SEP,
	TRMENU_LAYER_OPT_OPTION, TRMENU_LAYER_OPT_TYPE, TRMENU_LAYER_OPT_COLOR, MMENU_ARRAY16_SEP,
	TRMENU_LAYER_FOLDER_CHECKED_MOVE, TRMENU_LAYER_FOLDER_CLOSE_EX_CUR, MMENU_ARRAY16_SEP,
	TRMENU_LAYER_FLAG_OFF_FILL, TRMENU_LAYER_FLAG_OFF_CHECKED,
	MMENU_ARRAY16_SEP,
	MMENU_ARRAY16_END
};

//合成モード名

static const char *g_blend_name[] = {
	"NORM", "MUL", "ADD", "SUB", "SCRN", "OVERL", "HARDL", "SOFTL",
	"DODGE", "BURN", "LBURN", "VIVID", "LINRL", "PINL",
	"DARK", "LIGHT", "DIFF", "LMADD", "LMDOD"
};

//----------- 画像データ

//フォルダアイコン (1bit, 10x9)

static const unsigned char g_img_folder_icon[]={
0x3f,0x00,0x3f,0x00,0x00,0x00,0xff,0x03,0xff,0x03,0xff,0x03,0xff,0x03,0xff,0x03,0xff,0x03 };

//フォルダ矢印

static const unsigned char g_img_arrow_right[9] =
	{ 0x01,0x03,0x07,0x0f,0x1f,0x0f,0x07,0x03,0x01 };
static const unsigned char g_img_arrow_down[10] =
	{ 0xff,0x01,0xfe,0x00,0x7c,0x00,0x38,0x00,0x10,0x00 };

//数字 (不透明度, トーン情報) (1bit, [5x7]x10)
static const unsigned char g_img_number[]={
0x9e,0x79,0x2f,0xbd,0xf7,0xde,0x0b,0x12,0x41,0x28,0x85,0x90,0x52,0x0a,0x12,0x41,
0x28,0x85,0x90,0x52,0x0a,0x12,0x79,0xef,0xbd,0x87,0xde,0x0b,0x12,0x09,0x08,0xa1,
0x84,0x12,0x0a,0x12,0x09,0x08,0xa1,0x84,0x12,0x0a,0x9e,0x7b,0x0f,0xbd,0x87,0x1e,
0x7a };

//カラータイプ:GRAY (8bit, 14x14)

static const unsigned char g_img_coltype_gray[]={
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x17,0x2e,0x45,0x5c,0x73,0x8b,0xa2,0xb9,0xd0,0xe7,0xff,0x00,0x00,0x00,0x17,0x2e,
0x45,0x5c,0x73,0x8b,0xa2,0xb9,0xd0,0xe7,0xff,0x00,0x00,0x00,0x17,0x2e,0x45,0x5c,
0x73,0x8b,0xa2,0xb9,0xd0,0xe7,0xff,0x00,0x00,0x00,0x17,0x2e,0x45,0x5c,0x73,0x8b,
0xa2,0xb9,0xd0,0xe7,0xff,0x00,0x00,0x00,0x17,0x2e,0x45,0x5c,0x73,0x8b,0xa2,0xb9,
0xd0,0xe7,0xff,0x00,0x00,0x00,0x17,0x2e,0x45,0x5c,0x73,0x8b,0xa2,0xb9,0xd0,0xe7,
0xff,0x00,0x00,0x00,0x17,0x2e,0x45,0x5c,0x73,0x8b,0xa2,0xb9,0xd0,0xe7,0xff,0x00,
0x00,0x00,0x17,0x2e,0x45,0x5c,0x73,0x8b,0xa2,0xb9,0xd0,0xe7,0xff,0x00,0x00,0x00,
0x17,0x2e,0x45,0x5c,0x73,0x8b,0xa2,0xb9,0xd0,0xe7,0xff,0x00,0x00,0x00,0x17,0x2e,
0x45,0x5c,0x73,0x8b,0xa2,0xb9,0xd0,0xe7,0xff,0x00,0x00,0x00,0x17,0x2e,0x45,0x5c,
0x73,0x8b,0xa2,0xb9,0xd0,0xe7,0xff,0x00,0x00,0x00,0x17,0x2e,0x45,0x5c,0x73,0x8b,
0xa2,0xb9,0xd0,0xe7,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00 };

//カラータイプ:A (1bit)

static const unsigned char g_img_coltype_alpha[]={
0xff,0x3f,0x01,0x20,0x01,0x20,0x01,0x20,0x01,0x20,0x01,0x20,0x01,0x20,0x01,0x20,
0x01,0x20,0x01,0x20,0x01,0x20,0x01,0x20,0x01,0x20,0xff,0x3f };

//カラータイプ:A1 (2bit)

static const unsigned char g_img_coltype_a1[]={
0x00,0x00,0x00,0x00,0x54,0x55,0x55,0x01,0x54,0x55,0x55,0x01,0x54,0x55,0x55,0x01,
0x54,0x55,0x55,0x01,0x54,0x55,0x55,0x01,0x54,0x55,0x55,0x01,0x54,0x55,0x01,0x00,
0x54,0x55,0xf1,0x00,0x54,0x55,0xc1,0x00,0x54,0x55,0xc1,0x00,0x54,0x55,0xc1,0x00,
0x54,0x55,0xf1,0x03,0x00,0x00,0x00,0x00 };

//カラータイプ:GRAY-TONE (2bit)

static const unsigned char g_img_coltype_tone_gray[]={
0x00,0x00,0x00,0x00,0x54,0x55,0x55,0x01,0x54,0x55,0x55,0x01,0x54,0x55,0x55,0x01,
0x54,0x55,0x55,0x01,0x54,0x55,0x55,0x01,0x00,0x00,0x55,0x01,0x00,0x3f,0x55,0x01,
0x00,0x3f,0x55,0x01,0x00,0x3f,0x55,0x01,0xfc,0x00,0x55,0x01,0xfc,0x00,0x55,0x01,
0xfc,0x00,0x55,0x01,0x00,0x00,0x00,0x00 };

//カラータイプ:A1-TONE (2bit)

static const unsigned char g_img_coltype_tone_a1[]={
0x00,0x00,0x00,0x00,0x54,0x55,0x55,0x01,0x54,0x55,0x55,0x01,0x54,0x55,0x55,0x01,
0x54,0x55,0x55,0x01,0x54,0x55,0x55,0x01,0x00,0x00,0x01,0x00,0x00,0x3f,0xf1,0x00,
0x00,0x3f,0xc1,0x00,0x00,0x3f,0xc1,0x00,0xfc,0x00,0xc1,0x00,0xfc,0x00,0xc1,0x00,
0xfc,0x00,0xf1,0x03,0x00,0x00,0x00,0x00 };

//テキストレイヤ (1bit, 6x7)

static const unsigned char g_img_coltype_text[]={
0x3f,0x20,0x2a,0x3b,0x3b,0x3b,0x31 };

//表示状態ボックス (2bit, [13x13] x 2)

static const unsigned char g_img_visible_2bit[]={
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x50,0x55,0x15,0x50,0x55,0x55,0x01,0x44,0x55,
0x45,0x90,0xaa,0xaa,0x01,0x14,0x55,0x51,0x90,0xaa,0xaa,0x01,0x54,0x54,0x54,0x90,
0xaa,0xaa,0x01,0x54,0x11,0x55,0x90,0xaa,0xaa,0x01,0x54,0x45,0x55,0x90,0xaa,0xaa,
0x01,0x54,0x11,0x55,0x90,0xaa,0xaa,0x01,0x54,0x54,0x54,0x90,0xaa,0xaa,0x01,0x14,
0x55,0x51,0x90,0xaa,0xaa,0x01,0x44,0x55,0x45,0x90,0xaa,0xaa,0x01,0x50,0x55,0x15,
0x50,0x55,0x55,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };

//フラグボックス (2bit, [14x14] x 6)

#define _FLAGBOX_IMG_WIDTH  (14*6)

static const unsigned char g_img_flags_2bit[]={
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0xfc,0xff,0xff,0xc3,0xff,0xff,0x3f,0xfc,0xff,0xff,0xc3,
0xff,0xff,0x3f,0xfc,0xff,0xff,0xc3,0xff,0xff,0x3f,0xfc,0x7f,0xff,0xc3,0xff,0xff,
0x3f,0xfc,0x5f,0xff,0xc3,0xff,0xff,0x3f,0xfc,0x5f,0xff,0xc3,0x55,0x55,0x3d,0xfc,
0xdf,0xfd,0xc3,0x7f,0xd5,0x3f,0xfc,0xf7,0xfd,0xc3,0xff,0xff,0x37,0xfc,0x5f,0xff,
0xc3,0xfd,0xff,0x3d,0xfc,0xf7,0xf7,0xc3,0x5f,0x55,0x3f,0xfc,0xf7,0xfd,0xc3,0xff,
0xff,0x35,0xfc,0x57,0xfd,0xc3,0x7d,0xf5,0x3d,0xfc,0xfd,0xdf,0xc3,0x57,0x55,0x3d,
0x7c,0x55,0xd5,0xc3,0xfd,0x7f,0x35,0xfc,0x57,0xfd,0xc3,0x5d,0xd5,0x3d,0x7c,0xff,
0x7f,0xc3,0x57,0x55,0x3d,0x7c,0xff,0xdf,0xc3,0xf5,0x5f,0x3d,0xfc,0xf5,0xf5,0xc3,
0x5d,0xd5,0x3d,0x7c,0x55,0x55,0xc1,0x57,0x55,0x3d,0x7c,0xff,0xdf,0xc3,0xd5,0x57,
0x3f,0xfc,0xf5,0xf5,0xc3,0x5d,0xd5,0x3d,0x7c,0x57,0x55,0xc3,0x57,0x55,0x3d,0x7c,
0x5f,0xdf,0xc3,0x57,0xd5,0x3f,0x7c,0x55,0xd5,0xc3,0x7d,0xff,0x3f,0x7c,0x5f,0xd5,
0xc3,0x5f,0x55,0x3f,0x7c,0x5f,0xdf,0xc3,0x5f,0xf5,0x3f,0x7c,0x55,0xd5,0xc3,0xfd,
0x57,0x35,0x7c,0x7f,0xf5,0xc3,0x7f,0xd5,0x3f,0x7c,0xff,0xdf,0xc3,0x7f,0xfd,0x3f,
0x5c,0xff,0x5f,0xc3,0x55,0x5f,0x3d,0x5c,0xff,0xfd,0xc3,0xff,0xff,0x3f,0x7c,0x55,
0xd5,0xc3,0xff,0xff,0x3f,0x5c,0xff,0x5f,0xc3,0xff,0x7f,0x3f,0xfc,0xff,0xff,0xc3,
0xff,0xff,0x3f,0xfc,0xff,0xff,0xc3,0xff,0xff,0x3f,0xfc,0xff,0xff,0xc3,0xff,0xff,
0x3f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00 };

//合成モード用アルファベット ([6x7] x 26)

static const unsigned char g_img_alphabet[]={
0xc4,0xe3,0x1c,0xdf,0xe7,0x44,0x8e,0x17,0x05,0x51,0xe4,0x3c,0xce,0xe3,0x7c,0x51,
0x14,0x45,0xd1,0x07,0x44,0x14,0x25,0x41,0x10,0x45,0x04,0x92,0x04,0xdb,0x14,0x45,
0x51,0x14,0x11,0x51,0x14,0x45,0x11,0x04,0x4a,0x14,0x45,0x41,0x10,0x44,0x04,0x52,
0x04,0xdb,0x14,0x45,0x51,0x14,0x10,0x51,0x54,0x29,0x11,0x02,0xca,0x13,0x44,0xdf,
0xd3,0x7d,0x04,0x32,0x04,0x5b,0x15,0x3d,0xd1,0xe3,0x10,0x91,0x52,0x11,0x0a,0x01,
0x5f,0x14,0x44,0x41,0x10,0x45,0x44,0x52,0x04,0x55,0x16,0x05,0x55,0x02,0x11,0x91,
0xf2,0x29,0x84,0x00,0x51,0x14,0x25,0x41,0x10,0x45,0x44,0x92,0x04,0x51,0x16,0x05,
0x59,0x14,0x11,0x11,0xe1,0x44,0x44,0x00,0xd1,0xe3,0x1c,0x5f,0xe0,0x45,0x8e,0x11,
0x7d,0x51,0xe4,0x04,0x5e,0xe4,0x10,0x0e,0xe1,0x44,0xc4,0x07 };
