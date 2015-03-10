#include "stdafx.h"
#include "opencv_lib.hpp"							// OpenCVヘッダ

#define ColorN 3

// グレースケール変換
void ConvertToGray(Mat img_rgb[ColorN], Mat img_gray[ColorN])
{
	int i;
	for (i=0;i<ColorN;i++){
		cvtColor(img_rgb[i], img_gray[i], CV_BGR2GRAY);
	}
}

// 正規化する関数(入力:src 出力:dst)
void normalize_255(Mat src[], Mat dst[]){

	for (int i = 0; i < 3; i++){
		normalize(src[i], dst[i], 0, 255, NORM_MINMAX);
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	Scalar average;
	Mat frame, img_gray[ColorN], img_rgb[ColorN], img_rgb_3[ColorN];	// 画像リソース宣言
	VideoCapture src(-1);							// 映像取得（カメラ映像）

	if (src.isOpened() == 0){ cout << "映像が取得できません。\n" << endl; waitKey(0); return -1; }

	while (frame.data == NULL){ src >> frame; }		// 初期フレーム取得
	// 画像リソース確保
	img_rgb[0].create(frame.size(), CV_8UC1);		// 赤1チャンネル
	img_rgb[1].create(frame.size(), CV_8UC1);		// 緑1チャンネル
	img_rgb[2].create(frame.size(), CV_8UC1);		// 青1チャンネル

	img_rgb_3[0].create(frame.size(), CV_8UC3);		// 赤3チャンネル
	img_rgb_3[1].create(frame.size(), CV_8UC3);		// 緑3チャンネル
	img_rgb_3[2].create(frame.size(), CV_8UC3);		// 青3チャンネル

	img_rgb_3[0].setTo(0); img_rgb_3[1].setTo(0); img_rgb_3[2].setTo(0);	// 0で初期化

	while (1){

		src >> frame; if (frame.data == NULL) break;	// 1フレーム取得

		split(frame, img_rgb);							// RGB分離

		int from_to_r[] = { 0, 2 };
		int from_to_g[] = { 0, 1 };
		int from_to_b[] = { 0, 0 };

		mixChannels(&img_rgb[2], 1, &img_rgb_3[2], 1, from_to_r, 1);	// 赤
		mixChannels(&img_rgb[1], 1, &img_rgb_3[1], 1, from_to_g, 1);	// 緑
		mixChannels(&img_rgb[0], 1, &img_rgb_3[0], 1, from_to_b, 1);	// 青

		ConvertToGray(img_rgb_3, img_gray);			// グレースケール変換

		// 正規化
		normalize_255(img_rgb, img_rgb);

		imshow("入力映像", frame);
		imshow("赤チャンネル", img_gray[2]);
		imshow("緑チャンネル", img_gray[1]);
		imshow("青チャンネル", img_gray[0]);

		average = mean(frame);						// チャンネルごとのピクセル平均値
		int r_ave = (int)average.val[2];
		int g_ave = (int)average.val[1];
		int b_ave = (int)average.val[0];
		printf("RGB平均値 = (%3d,%3d,%3d)：", r_ave, g_ave, b_ave); // 各成分の平均値

		// 各平均値を比較し最大成分を表示する
		if		((r_ave > g_ave) && (r_ave > b_ave)) cout << "赤成分が多い\n" << endl;
		else if ((g_ave > r_ave) && (g_ave > b_ave)) cout << "緑成分が多い\n" << endl;
		else if ((b_ave > r_ave) && (b_ave > g_ave)) cout << "青成分が多い\n" << endl;
		else cout << "\n" << endl;

		if (waitKey(30) == 27) break;				// キー入力待機（30ms）
	}

	return 0;
}
