#include <Novice.h>
#include <cmath>
#include <cassert>
#include <imgui.h>
#define _USE_MATH_DEFINES
#include "math.h"

const char kWindowTitle[] = "LE2B_12_サクライショウセイ_タイトル";

struct Vector3 {
	float x, y, z;
};

struct Matrix4x4 {
	float m[4][4];
};

struct AABB {
	Vector3 min;
	Vector3 max;
};

Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);
Matrix4x4 Inverse(const Matrix4x4& m);
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);
Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);
bool IsCollision(AABB& a, AABB b);
void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);
void DrawAABB(const AABB& aabb, Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportmatrx, uint32_t color);

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);
	Vector3 cameraTranslate{ 0.0f,1.9f,-6.49f };
	Vector3 cameraRotate{ 0.26f,0.0f,0.0f };

	AABB aabb1 = {};
	AABB aabb2 = {};

	uint32_t AABBColor = WHITE;

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		ImGui::Begin("Window");
		ImGui::DragFloat3("CameraTranslate", &cameraTranslate.x, 0.01f);
		ImGui::DragFloat3("CameraRotate", &cameraRotate.x, 0.01f);
		ImGui::DragFloat3("AABB1.min", &aabb1.min.x, 0.1f);
		ImGui::DragFloat3("AABB1.max", &aabb1.max.x, 0.1f);
		ImGui::DragFloat3("AABB2.min", &aabb2.min.x, 0.1f);
		ImGui::DragFloat3("AABB2.max", &aabb2.max.x, 0.1f);
		ImGui::End();

		Matrix4x4 cameraMatrix = MakeAffineMatrix({ 1.0f, 1.0f,1.0f }, cameraRotate, cameraTranslate);

		Matrix4x4 viewMatrix = Inverse(cameraMatrix);

		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, 1280.0f / 720.0f, 0.1f, 100.f);

		Matrix4x4 viewportMatrix = MakeViewportMatrix(0, 0, 1280.0f, 720.0f, 0.0f, 1.0f);

		Matrix4x4 viewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);

		if (IsCollision(aabb1, aabb2)) {
			AABBColor = RED;
		} else {
			AABBColor = WHITE;
		}

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		DrawGrid(viewProjectionMatrix, viewportMatrix);

		DrawAABB(aabb1, viewProjectionMatrix, viewportMatrix, AABBColor);
		DrawAABB(aabb2, viewProjectionMatrix, viewportMatrix, WHITE);

		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}

Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 a;
	a.m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[0][1] * m2.m[1][0] + m1.m[0][2] * m2.m[2][0] + m1.m[0][3] * m2.m[3][0];
	a.m[0][1] = m1.m[0][0] * m2.m[0][1] + m1.m[0][1] * m2.m[1][1] + m1.m[0][2] * m2.m[2][1] + m1.m[0][3] * m2.m[3][1];
	a.m[0][2] = m1.m[0][0] * m2.m[0][2] + m1.m[0][1] * m2.m[1][2] + m1.m[0][2] * m2.m[2][2] + m1.m[0][3] * m2.m[3][2];
	a.m[0][3] = m1.m[0][0] * m2.m[0][3] + m1.m[0][1] * m2.m[1][3] + m1.m[0][2] * m2.m[2][3] + m1.m[0][3] * m2.m[3][3];

	a.m[1][0] = m1.m[1][0] * m2.m[0][0] + m1.m[1][1] * m2.m[1][0] + m1.m[1][2] * m2.m[2][0] + m1.m[1][3] * m2.m[3][0];
	a.m[1][1] = m1.m[1][0] * m2.m[0][1] + m1.m[1][1] * m2.m[1][1] + m1.m[1][2] * m2.m[2][1] + m1.m[1][3] * m2.m[3][1];
	a.m[1][2] = m1.m[1][0] * m2.m[0][2] + m1.m[1][1] * m2.m[1][2] + m1.m[1][2] * m2.m[2][2] + m1.m[1][3] * m2.m[3][2];
	a.m[1][3] = m1.m[1][0] * m2.m[0][3] + m1.m[1][1] * m2.m[1][3] + m1.m[1][2] * m2.m[2][3] + m1.m[1][3] * m2.m[3][3];

	a.m[2][0] = m1.m[2][0] * m2.m[0][0] + m1.m[2][1] * m2.m[1][0] + m1.m[2][2] * m2.m[2][0] + m1.m[2][3] * m2.m[3][0];
	a.m[2][1] = m1.m[2][0] * m2.m[0][1] + m1.m[2][1] * m2.m[1][1] + m1.m[2][2] * m2.m[2][1] + m1.m[2][3] * m2.m[3][1];
	a.m[2][2] = m1.m[2][0] * m2.m[0][2] + m1.m[2][1] * m2.m[1][2] + m1.m[2][2] * m2.m[2][2] + m1.m[2][3] * m2.m[3][2];
	a.m[2][3] = m1.m[2][0] * m2.m[0][3] + m1.m[2][1] * m2.m[1][3] + m1.m[2][2] * m2.m[2][3] + m1.m[2][3] * m2.m[3][3];

	a.m[3][0] = m1.m[3][0] * m2.m[0][0] + m1.m[3][1] * m2.m[1][0] + m1.m[3][2] * m2.m[2][0] + m1.m[3][3] * m2.m[3][0];
	a.m[3][1] = m1.m[3][0] * m2.m[0][1] + m1.m[3][1] * m2.m[1][1] + m1.m[3][2] * m2.m[2][1] + m1.m[3][3] * m2.m[3][1];
	a.m[3][2] = m1.m[3][0] * m2.m[0][2] + m1.m[3][1] * m2.m[1][2] + m1.m[3][2] * m2.m[2][2] + m1.m[3][3] * m2.m[3][2];
	a.m[3][3] = m1.m[3][0] * m2.m[0][3] + m1.m[3][1] * m2.m[1][3] + m1.m[3][2] * m2.m[2][3] + m1.m[3][3] * m2.m[3][3];
	return a;
}

Matrix4x4 MakeRotateXMatrix(float radian) {
	Matrix4x4 a;
	a.m[0][0] = 1;
	a.m[0][1] = 0;
	a.m[0][2] = 0;
	a.m[0][3] = 0;
	a.m[1][0] = 0;
	a.m[1][1] = std::cos(radian);
	a.m[1][2] = std::sin(radian);
	a.m[1][3] = 0;
	a.m[2][0] = 0;
	a.m[2][1] = -std::sin(radian);
	a.m[2][2] = std::cos(radian);
	a.m[2][3] = 0;
	a.m[3][0] = 0;
	a.m[3][1] = 0;
	a.m[3][2] = 0;
	a.m[3][3] = 1;
	return a;
}

Matrix4x4 MakeRotateYMatrix(float radian) {
	Matrix4x4 a;
	a.m[0][0] = std::cos(radian);
	a.m[0][1] = 0;
	a.m[0][2] = -std::sin(radian);
	a.m[0][3] = 0;
	a.m[1][0] = 0;
	a.m[1][1] = 1;
	a.m[1][2] = 0;
	a.m[1][3] = 0;
	a.m[2][0] = std::sin(radian);
	a.m[2][1] = 0;
	a.m[2][2] = std::cos(radian);
	a.m[2][3] = 0;
	a.m[3][0] = 0;
	a.m[3][1] = 0;
	a.m[3][2] = 0;
	a.m[3][3] = 1;
	return a;
}

Matrix4x4 MakeRotateZMatrix(float radian) {
	Matrix4x4 a;
	a.m[0][0] = std::cos(radian);
	a.m[0][1] = std::sin(radian);
	a.m[0][2] = 0;
	a.m[0][3] = 0;
	a.m[1][0] = -std::sin(radian);
	a.m[1][1] = std::cos(radian);
	a.m[1][2] = 0;
	a.m[1][3] = 0;
	a.m[2][0] = 0;
	a.m[2][1] = 0;
	a.m[2][2] = 1;
	a.m[2][3] = 0;
	a.m[3][0] = 0;
	a.m[3][1] = 0;
	a.m[3][2] = 0;
	a.m[3][3] = 1;
	return a;
}

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	Matrix4x4 a;
	Matrix4x4 b;

	b = Multiply(MakeRotateXMatrix(rotate.x), Multiply(MakeRotateYMatrix(rotate.y), MakeRotateZMatrix(rotate.z)));
	a.m[0][0] = scale.x * b.m[0][0];
	a.m[0][1] = scale.x * b.m[0][1];
	a.m[0][2] = scale.x * b.m[0][2];
	a.m[0][3] = 0;
	a.m[1][0] = scale.y * b.m[1][0];
	a.m[1][1] = scale.y * b.m[1][1];
	a.m[1][2] = scale.y * b.m[1][2];
	a.m[1][3] = 0;
	a.m[2][0] = scale.z * b.m[2][0];
	a.m[2][1] = scale.z * b.m[2][1];
	a.m[2][2] = scale.z * b.m[2][2];
	a.m[2][3] = 0;
	a.m[3][0] = translate.x;
	a.m[3][1] = translate.y;
	a.m[3][2] = translate.z;
	a.m[3][3] = 1;

	return a;
}

Matrix4x4 Inverse(const Matrix4x4& m)
{
	Matrix4x4 a;
	float detA =
		m.m[0][0] * m.m[1][1] * m.m[2][2] * m.m[3][3] +
		m.m[0][0] * m.m[1][2] * m.m[2][3] * m.m[3][1] +
		m.m[0][0] * m.m[1][3] * m.m[2][1] * m.m[3][2] -

		m.m[0][0] * m.m[1][3] * m.m[2][2] * m.m[3][1] -
		m.m[0][0] * m.m[1][2] * m.m[2][1] * m.m[3][3] -
		m.m[0][0] * m.m[1][1] * m.m[2][3] * m.m[3][2] -

		m.m[0][1] * m.m[1][0] * m.m[2][2] * m.m[3][3] -
		m.m[0][2] * m.m[1][0] * m.m[2][3] * m.m[3][1] -
		m.m[0][3] * m.m[1][0] * m.m[2][1] * m.m[3][2] +

		m.m[0][3] * m.m[1][0] * m.m[2][2] * m.m[3][1] +
		m.m[0][2] * m.m[1][0] * m.m[2][1] * m.m[3][3] +
		m.m[0][1] * m.m[1][0] * m.m[2][3] * m.m[3][2] +

		m.m[0][1] * m.m[1][2] * m.m[2][0] * m.m[3][3] +
		m.m[0][2] * m.m[1][3] * m.m[2][0] * m.m[3][1] +
		m.m[0][3] * m.m[1][1] * m.m[2][0] * m.m[3][2] -

		m.m[0][3] * m.m[1][2] * m.m[2][0] * m.m[3][1] -
		m.m[0][2] * m.m[1][1] * m.m[2][0] * m.m[3][3] -
		m.m[0][1] * m.m[1][3] * m.m[2][0] * m.m[3][2] -

		m.m[0][1] * m.m[1][2] * m.m[2][3] * m.m[3][0] -
		m.m[0][2] * m.m[1][3] * m.m[2][1] * m.m[3][0] -
		m.m[0][3] * m.m[1][1] * m.m[2][2] * m.m[3][0] +

		m.m[0][3] * m.m[1][2] * m.m[2][1] * m.m[3][0] +
		m.m[0][2] * m.m[1][1] * m.m[2][3] * m.m[3][0] +
		m.m[0][1] * m.m[1][3] * m.m[2][2] * m.m[3][0];

	a.m[0][0] = 1 / detA * (m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[1][2] * m.m[2][3] * m.m[3][1] + m.m[1][3] * m.m[2][1] * m.m[3][2] - m.m[1][3] * m.m[2][2] * m.m[3][1] - m.m[1][2] * m.m[2][1] * m.m[3][3] - m.m[1][1] * m.m[2][3] * m.m[3][2]);
	a.m[0][1] = 1 / detA * (-m.m[0][1] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[2][3] * m.m[3][1] - m.m[0][3] * m.m[2][1] * m.m[3][2] + m.m[0][3] * m.m[2][2] * m.m[3][1] + m.m[0][2] * m.m[2][1] * m.m[3][3] + m.m[0][1] * m.m[2][3] * m.m[3][2]);
	a.m[0][2] = 1 / detA * (m.m[0][1] * m.m[1][2] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[3][1] + m.m[0][3] * m.m[1][1] * m.m[3][2] - m.m[0][3] * m.m[1][2] * m.m[3][1] - m.m[0][2] * m.m[1][1] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[3][2]);
	a.m[0][3] = 1 / detA * (-m.m[0][1] * m.m[1][2] * m.m[2][3] - m.m[0][2] * m.m[1][3] * m.m[2][1] - m.m[0][3] * m.m[1][1] * m.m[2][2] + m.m[0][3] * m.m[1][2] * m.m[2][1] + m.m[0][2] * m.m[1][1] * m.m[2][3] + m.m[0][1] * m.m[1][3] * m.m[2][2]);

	a.m[1][0] = 1 / detA * -(m.m[1][0] * m.m[2][2] * m.m[3][3] + m.m[1][2] * m.m[2][3] * m.m[3][0] + m.m[1][3] * m.m[2][0] * m.m[3][2] - m.m[1][3] * m.m[2][2] * m.m[3][0] - m.m[1][2] * m.m[2][0] * m.m[3][3] - m.m[1][0] * m.m[2][3] * m.m[3][2]);
	a.m[1][1] = 1 / detA * -(-m.m[0][0] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[2][3] * m.m[3][0] - m.m[0][3] * m.m[2][0] * m.m[3][2] + m.m[0][3] * m.m[2][2] * m.m[3][0] + m.m[0][2] * m.m[2][0] * m.m[3][3] + m.m[0][0] * m.m[2][3] * m.m[3][2]);
	a.m[1][2] = 1 / detA * -(m.m[0][0] * m.m[1][2] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[3][0] + m.m[0][3] * m.m[1][0] * m.m[3][2] - m.m[0][3] * m.m[1][2] * m.m[3][0] - m.m[0][2] * m.m[1][0] * m.m[3][3] - m.m[0][0] * m.m[1][3] * m.m[3][2]);
	a.m[1][3] = 1 / detA * -(-m.m[0][0] * m.m[1][2] * m.m[2][3] - m.m[0][2] * m.m[1][3] * m.m[2][0] - m.m[0][3] * m.m[1][0] * m.m[2][2] + m.m[0][3] * m.m[1][2] * m.m[2][0] + m.m[0][2] * m.m[1][0] * m.m[2][3] + m.m[0][0] * m.m[1][3] * m.m[2][2]);

	a.m[2][0] = 1 / detA * (m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[1][1] * m.m[2][3] * m.m[3][0] + m.m[1][3] * m.m[2][0] * m.m[3][1] - m.m[1][3] * m.m[2][1] * m.m[3][0] - m.m[1][1] * m.m[2][0] * m.m[3][3] - m.m[1][0] * m.m[2][3] * m.m[3][1]);
	a.m[2][1] = 1 / detA * (-m.m[0][0] * m.m[2][1] * m.m[3][3] - m.m[0][1] * m.m[2][3] * m.m[3][0] - m.m[0][3] * m.m[2][0] * m.m[3][1] + m.m[0][3] * m.m[2][1] * m.m[3][0] + m.m[0][1] * m.m[2][0] * m.m[3][3] + m.m[0][0] * m.m[2][3] * m.m[3][1]);
	a.m[2][2] = 1 / detA * (m.m[0][0] * m.m[1][1] * m.m[3][3] + m.m[0][1] * m.m[1][3] * m.m[3][0] + m.m[0][3] * m.m[1][0] * m.m[3][1] - m.m[0][3] * m.m[1][1] * m.m[3][0] - m.m[0][1] * m.m[1][0] * m.m[3][3] - m.m[0][0] * m.m[1][3] * m.m[3][1]);
	a.m[2][3] = 1 / detA * (-m.m[0][0] * m.m[1][1] * m.m[2][3] - m.m[0][1] * m.m[1][3] * m.m[2][0] - m.m[0][3] * m.m[1][0] * m.m[2][1] + m.m[0][3] * m.m[1][1] * m.m[2][0] + m.m[0][1] * m.m[1][0] * m.m[2][3] + m.m[0][0] * m.m[1][3] * m.m[2][1]);

	a.m[3][0] = 1 / detA * -(m.m[1][0] * m.m[2][1] * m.m[3][2] + m.m[1][1] * m.m[2][2] * m.m[3][0] + m.m[1][2] * m.m[2][0] * m.m[3][1] - m.m[1][2] * m.m[2][1] * m.m[3][0] - m.m[1][1] * m.m[2][0] * m.m[3][2] - m.m[1][0] * m.m[2][2] * m.m[3][1]);
	a.m[3][1] = 1 / detA * -(-m.m[0][0] * m.m[2][1] * m.m[3][2] - m.m[0][1] * m.m[2][2] * m.m[3][0] - m.m[0][2] * m.m[2][0] * m.m[3][1] + m.m[0][2] * m.m[2][1] * m.m[3][0] + m.m[0][1] * m.m[2][0] * m.m[3][2] + m.m[0][0] * m.m[2][2] * m.m[3][1]);
	a.m[3][2] = 1 / detA * -(m.m[0][0] * m.m[1][1] * m.m[3][2] + m.m[0][1] * m.m[1][2] * m.m[3][0] + m.m[0][2] * m.m[1][0] * m.m[3][1] - m.m[0][2] * m.m[1][1] * m.m[3][0] - m.m[0][1] * m.m[1][0] * m.m[3][2] - m.m[0][0] * m.m[1][2] * m.m[3][1]);
	a.m[3][3] = 1 / detA * -(-m.m[0][0] * m.m[1][1] * m.m[2][2] - m.m[0][1] * m.m[1][2] * m.m[2][0] - m.m[0][2] * m.m[1][0] * m.m[2][1] + m.m[0][2] * m.m[1][1] * m.m[2][0] + m.m[0][1] * m.m[1][0] * m.m[2][2] + m.m[0][0] * m.m[1][2] * m.m[2][1]);

	return a;
}

Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip)
{
	Matrix4x4 a;
	a.m[0][0] = 1 / aspectRatio * 1 / std::tan(fovY / 2);
	a.m[0][1] = 0;
	a.m[0][2] = 0;
	a.m[0][3] = 0;
	a.m[1][0] = 0;
	a.m[1][1] = 1 / std::tan(fovY / 2);
	a.m[1][2] = 0;
	a.m[1][3] = 0;
	a.m[2][0] = 0;
	a.m[2][1] = 0;
	a.m[2][2] = farClip / (farClip - nearClip);
	a.m[2][3] = 1;
	a.m[3][0] = 0;
	a.m[3][1] = 0;
	a.m[3][2] = -nearClip * farClip / (farClip - nearClip);
	a.m[3][3] = 0;
	return a;
}

Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth)
{
	Matrix4x4 a;
	a.m[0][0] = width / 2;
	a.m[0][1] = 0;
	a.m[0][2] = 0;
	a.m[0][3] = 0;
	a.m[1][0] = 0;
	a.m[1][1] = -height / 2;
	a.m[1][2] = 0;
	a.m[1][3] = 0;
	a.m[2][0] = 0;
	a.m[2][1] = 0;
	a.m[2][2] = maxDepth - minDepth;
	a.m[2][3] = 0;
	a.m[3][0] = left + width / 2;
	a.m[3][1] = top + height / 2;
	a.m[3][2] = minDepth;
	a.m[3][3] = 1;
	return a;
}

Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix)
{
	Vector3 b;
	b.x = (vector.x * matrix.m[0][0]) + (vector.y * matrix.m[1][0]) + (vector.z * matrix.m[2][0]) + (1.0f * matrix.m[3][0]);
	b.y = (vector.x * matrix.m[0][1]) + (vector.y * matrix.m[1][1]) + (vector.z * matrix.m[2][1]) + (1.0f * matrix.m[3][1]);
	b.z = (vector.x * matrix.m[0][2]) + (vector.y * matrix.m[1][2]) + (vector.z * matrix.m[2][2]) + (1.0f * matrix.m[3][2]);
	float w = (vector.x * matrix.m[0][3]) + (vector.y * matrix.m[1][3]) + (vector.z * matrix.m[2][3]) + (1.0f * matrix.m[3][3]);
	assert(w != 0.0f);
	b.x /= w;
	b.y /= w;
	b.z /= w;
	return b;
}

bool IsCollision(AABB& a, AABB b)
{
	if ((a.min.x <= b.max.x && a.max.x >= b.min.x) &&
		(a.min.y <= b.max.y && a.max.y >= b.min.y) &&
		(a.min.z <= b.max.z && a.max.z >= b.min.z)) {

		return true;
	} else {
		return false;
	}
}

void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	const float kGridHalfWidth = 2.0f;
	const uint32_t kSubdivision = 10;
	const float kGridEvery = (kGridHalfWidth * 2.0f) / float(kSubdivision);
	// 奥から手前への線を順々に引いていく
	for (uint32_t xIndex = 0; xIndex <= kSubdivision; ++xIndex) {

		Matrix4x4 startWorldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, { (xIndex * kGridEvery),0.0f,kGridHalfWidth });
		Matrix4x4 endWorldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, { (xIndex * kGridEvery),0.0f,-kGridHalfWidth });

		Matrix4x4 startWorldViewProjectionMatrix = Multiply(startWorldMatrix, viewProjectionMatrix);
		Matrix4x4 endWorldViewProjectionMatrix = Multiply(endWorldMatrix, viewProjectionMatrix);

		Vector3 ndcStartVertex = Transform({ -2,0,0 }, startWorldViewProjectionMatrix);
		Vector3 ndcEndVertex = Transform({ -2,0,0 }, endWorldViewProjectionMatrix);

		Vector3 screenStartVertex = Transform(ndcStartVertex, viewportMatrix);
		Vector3 screenEndVertex = Transform(ndcEndVertex, viewportMatrix);
		Novice::DrawLine(int(screenStartVertex.x), int(screenStartVertex.y), int(screenEndVertex.x), int(screenEndVertex.y), 0xAAAAAAFF);
	}

	for (uint32_t zIndex = 0; zIndex <= kSubdivision; ++zIndex) {

		Matrix4x4 startWorldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, { kGridHalfWidth,0.0f,(zIndex * kGridEvery) });
		Matrix4x4 endWorldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, { -kGridHalfWidth,0.0f,(zIndex * kGridEvery) });

		Matrix4x4 startWorldViewProjectionMatrix = Multiply(startWorldMatrix, viewProjectionMatrix);
		Matrix4x4 endWorldViewProjectionMatrix = Multiply(endWorldMatrix, viewProjectionMatrix);

		Vector3 ndcStartVertex = Transform({ 0,0,-2 }, startWorldViewProjectionMatrix);
		Vector3 ndcEndVertex = Transform({ 0,0,-2 }, endWorldViewProjectionMatrix);

		Vector3 screenStartVertex = Transform(ndcStartVertex, viewportMatrix);
		Vector3 screenEndVertex = Transform(ndcEndVertex, viewportMatrix);

		Novice::DrawLine(int(screenStartVertex.x), int(screenStartVertex.y), int(screenEndVertex.x), int(screenEndVertex.y), 0xAAAAAAFF);
	}
}

void DrawAABB(const AABB& aabb, Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportmatrx, uint32_t color)
{
	Vector3 minXYZ = aabb.min;
	Vector3 minXYmaxZ = { aabb.min.x,aabb.min.y,aabb.max.z };
	Vector3 minYZmaxX = { aabb.max.x,aabb.min.y,aabb.min.z };
	Vector3 minYmaxXZ = { aabb.max.x,aabb.min.y,aabb.max.z };

	Matrix4x4 minXYZWorldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, minXYZ);
	Matrix4x4 minXYmaxZWorldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, minXYmaxZ);
	Matrix4x4 minYZmaxXWorldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, minYZmaxX);
	Matrix4x4 minYmaxXZWorldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, minYmaxXZ);

	Matrix4x4 minXYZWorldViewProjectionMatrix = Multiply(minXYZWorldMatrix, viewProjectionMatrix);
	Matrix4x4 minXYmaxZWorldViewProjectionMatrix = Multiply(minXYmaxZWorldMatrix, viewProjectionMatrix);
	Matrix4x4 minYZmaxXWorldViewProjectionMatrix = Multiply(minYZmaxXWorldMatrix, viewProjectionMatrix);
	Matrix4x4 minYmaxXZWorldViewProjectionMatrix = Multiply(minYmaxXZWorldMatrix, viewProjectionMatrix);

	Vector3 minXYZndcVertex = Transform({ 0,0,0 }, minXYZWorldViewProjectionMatrix);
	Vector3 minXYmaxZndcVertex = Transform({ 0,0,0 }, minXYmaxZWorldViewProjectionMatrix);
	Vector3 minYZmaxXndcVertex = Transform({ 0,0,0 }, minYZmaxXWorldViewProjectionMatrix);
	Vector3 minYmaxXZndcVertex = Transform({ 0,0,0 }, minYmaxXZWorldViewProjectionMatrix);

	Vector3 screenminXYZ = Transform(minXYZndcVertex, viewportmatrx);
	Vector3 screenminXYmaxZ = Transform(minXYmaxZndcVertex, viewportmatrx);
	Vector3 screenminYZmaxX = Transform(minYZmaxXndcVertex, viewportmatrx);
	Vector3 screenminYmaxXZ = Transform(minYmaxXZndcVertex, viewportmatrx);


	Vector3 maxXYZ = aabb.max;
	Vector3 maxXYminZ = { aabb.max.x,aabb.max.y,aabb.min.z };
	Vector3 maxYZminX = { aabb.min.x,aabb.max.y,aabb.max.z };
	Vector3 maxYminXZ = { aabb.min.x,aabb.max.y,aabb.min.z };

	// ワールド座標系
	Matrix4x4 maxXYZWorldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, maxXYZ);
	Matrix4x4 maxXYminZWorldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, maxXYminZ);
	Matrix4x4 maxYZminXWorldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, maxYZminX);
	Matrix4x4 maxYminXZWorldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, maxYminXZ);

	// 同時クリップ空間
	Matrix4x4 maxXYZWorldViewProjectionMatrix = Multiply(maxXYZWorldMatrix, viewProjectionMatrix);
	Matrix4x4 maxXYminZWorldViewProjectionMatrix = Multiply(maxXYminZWorldMatrix, viewProjectionMatrix);
	Matrix4x4 maxYZminXWorldViewProjectionMatrix = Multiply(maxYZminXWorldMatrix, viewProjectionMatrix);
	Matrix4x4 maxYminXZWorldViewProjectionMatrix = Multiply(maxYminXZWorldMatrix, viewProjectionMatrix);

	// 正規化デバイス座標系
	Vector3 maxXYZndcVertex = Transform({ 0,0,0 }, maxXYZWorldViewProjectionMatrix);
	Vector3 maxXYminZndcVertex = Transform({ 0,0,0 }, maxXYminZWorldViewProjectionMatrix);
	Vector3 maxYZminXndcVertex = Transform({ 0,0,0 }, maxYZminXWorldViewProjectionMatrix);
	Vector3 maxYminXZndcVertex = Transform({ 0,0,0 }, maxYminXZWorldViewProjectionMatrix);

	// スクリーン座標系
	Vector3 screenmaxXYZ = Transform(maxXYZndcVertex, viewportmatrx);
	Vector3 screenmaxXYminZ = Transform(maxXYminZndcVertex, viewportmatrx);
	Vector3 screenmaxYZminX = Transform(maxYZminXndcVertex, viewportmatrx);
	Vector3 screenmaxYminXZ = Transform(maxYminXZndcVertex, viewportmatrx);

	Novice::DrawLine(static_cast<int>(screenminXYZ.x), static_cast<int>(screenminXYZ.y), static_cast<int>(screenminXYmaxZ.x), static_cast<int>(screenminXYmaxZ.y), color);
	Novice::DrawLine(static_cast<int>(screenminXYZ.x), static_cast<int>(screenminXYZ.y), static_cast<int>(screenminYZmaxX.x), static_cast<int>(screenminYZmaxX.y), color);
	Novice::DrawLine(static_cast<int>(screenminXYZ.x), static_cast<int>(screenminXYZ.y), static_cast<int>(screenmaxYminXZ.x), static_cast<int>(screenmaxYminXZ.y), color);

	Novice::DrawLine(static_cast<int>(screenmaxYZminX.x), static_cast<int>(screenmaxYZminX.y), static_cast<int>(screenmaxYminXZ.x), static_cast<int>(screenmaxYminXZ.y), color);
	Novice::DrawLine(static_cast<int>(screenmaxYZminX.x), static_cast<int>(screenmaxYZminX.y), static_cast<int>(screenminXYmaxZ.x), static_cast<int>(screenminXYmaxZ.y), color);
	Novice::DrawLine(static_cast<int>(screenmaxYZminX.x), static_cast<int>(screenmaxYZminX.y), static_cast<int>(screenmaxXYZ.x), static_cast<int>(screenmaxXYZ.y), color);

	Novice::DrawLine(static_cast<int>(screenminYmaxXZ.x), static_cast<int>(screenminYmaxXZ.y), static_cast<int>(screenminXYmaxZ.x), static_cast<int>(screenminXYmaxZ.y), color);
	Novice::DrawLine(static_cast<int>(screenminYmaxXZ.x), static_cast<int>(screenminYmaxXZ.y), static_cast<int>(screenmaxXYZ.x), static_cast<int>(screenmaxXYZ.y), color);
	Novice::DrawLine(static_cast<int>(screenminYmaxXZ.x), static_cast<int>(screenminYmaxXZ.y), static_cast<int>(screenminYZmaxX.x), static_cast<int>(screenminYZmaxX.y), color);

	Novice::DrawLine(static_cast<int>(screenmaxXYminZ.x), static_cast<int>(screenmaxXYminZ.y), static_cast<int>(screenmaxXYZ.x), static_cast<int>(screenmaxXYZ.y), color);
	Novice::DrawLine(static_cast<int>(screenmaxXYminZ.x), static_cast<int>(screenmaxXYminZ.y), static_cast<int>(screenminYZmaxX.x), static_cast<int>(screenminYZmaxX.y), color);
	Novice::DrawLine(static_cast<int>(screenmaxXYminZ.x), static_cast<int>(screenmaxXYminZ.y), static_cast<int>(screenmaxYminXZ.x), static_cast<int>(screenmaxYminXZ.y), color);
}
